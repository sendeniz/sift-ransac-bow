#include "cvbow.h"
using namespace std;

static inline float Distance_Measure(const float* a, const float* b, int n)
{
	int j = 0; float d = 0.f;

	for(; j < n; j++ )
	{
		float t = a[j] - b[j];
		d += t*t;
	}
	return d;
}

/*
k-means center initialization using the following algorithm:
Arthur & Vassilvitskii (2007) k-means++: The Advantages of Careful Seeding
*/
static void GenerateCenters_PP(const cv::Mat& _data, cv::Mat& _out_centers, int K, cv::RNG& rng, int trials)
{
	int i, j, k, dims = _data.cols, N = _data.rows;
	const float* data = _data.ptr<float>(0);
	size_t step = _data.step/sizeof(data[0]);
	std::vector<int> _centers(K);
	int* centers = &_centers[0];
	std::vector<float> _dist(N*3);
	float* dist = &_dist[0], *tdist = dist + N, *tdist2 = tdist + N;
	double sum0 = 0;

	centers[0] = (unsigned)rng % N;

	for( i = 0; i < N; i++ )
	{
		dist[i] = Distance_Measure(data + step*i, data + step*centers[0], dims);
		sum0 += dist[i];
	}

	for( k = 1; k < K; k++ )
	{
		double bestSum = DBL_MAX;
		int bestCenter = -1;

		for( j = 0; j < trials; j++ )
		{
			double p = (double)rng*sum0, s = 0;
			for( i = 0; i < N-1; i++ )
				if( (p -= dist[i]) <= 0 )
					break;
			int ci = i;
			for( i = 0; i < N; i++ )
			{
				tdist2[i] = std::min(Distance_Measure(data + step*i, data + step*ci, dims), dist[i]);
				s += tdist2[i];
			}

			if( s < bestSum )
			{
				bestSum = s;
				bestCenter = ci;
				std::swap(tdist, tdist2);
			}
		}
		centers[k] = bestCenter;
		sum0 = bestSum;
		std::swap(dist, tdist);
	}

	for( k = 0; k < K; k++ )
	{
		const float* src = data + step*centers[k];
		float* dst = _out_centers.ptr<float>(k);
		for( j = 0; j < dims; j++ )
			dst[j] = src[j];
	}
}


void Flann_Search(double *_distances, int *_labels, const cv::Mat& _data, const cv::Mat& _centers)
{
	int Ncluster = _centers.rows;
	int Ndata    = _data.rows;
	cv::Mat results(Ndata, 1, CV_32S,cv::Scalar(0));
	cv::Mat dists(Ndata, 1, CV_32F, cv::Scalar(0.0));

	// Real value descriptors detected (CV_32F from SIFT, SURF) Create FLANN KDTree index
	cv::flann::Index flannIndex(_centers, cv::flann::KDTreeIndexParams(8), cvflann::FLANN_DIST_EUCLIDEAN);	
	flannIndex.knnSearch(_data, results, dists, 1, cv::flann::SearchParams(1024));// search one nearest neighbor for each feature
	for ( int i=0; i<Ndata; i++ )
	{
		_distances[i] = dists.at<float>(i,0);
		_labels[i] = results.at<int>(i,0);
	}
}

//flann based Kmeans_Cluster
double Kmeans_Cluster( cv::InputArray _data, int Numcluster, cv::OutputArray _centers )
{
	cv::Mat data = _data.getMat();
	int Num_data = data.rows;
	int dims     = data.cols;
	int type     = data.depth();

	CV_Assert( data.dims <= 2 && type == CV_32F && Numcluster > 0 );
	CV_Assert( Num_data >= Numcluster );

	cv::Mat _labels, best_labels;
	best_labels.create(Num_data, 1, CV_32S);
	_labels.create(best_labels.size(), best_labels.type());

	int* labels = _labels.ptr<int>();

	cv::Mat centers(Numcluster, dims, type);
	cv::Mat old_centers(Numcluster, dims, type);
	cv::Mat temp(1, dims, type);
	std::vector<int> counters(Numcluster);

	double best_compactness = DBL_MAX;
	double compactness = 0;
	cv::RNG& rng = cv::theRNG();
	int a, iter, i, j, k;

	const float* sample = data.ptr<float>(0);

	for( a = 0; a < 2; a++ )
	{
		cout<<a<<endl;
		double max_center_shift = DBL_MAX;
		for( iter = 0;; )
		{
			cout<<iter<<endl;
			swap(centers, old_centers);

			if( iter == 0 )
			{
				GenerateCenters_PP(data, centers, Numcluster, rng, 3);
			}
			else
			{			
				// compute centers
				centers = cv::Scalar(0);
				for( k = 0; k < Numcluster; k++ )
					counters[k] = 0;

				for( i = 0; i < Num_data; i++ )
				{
					sample = data.ptr<float>(i);
					k = labels[i];
					float* center = centers.ptr<float>(k);
					for( j=0 ; j < dims; j++ )
						center[j] += sample[j];
					counters[k]++;
				}

				if( iter > 0 )
					max_center_shift = 0;

				for( k = 0; k < Numcluster; k++ )
				{
					if( counters[k] != 0 )
						continue;

					// if some cluster appeared to be empty then:
					//   1. find the biggest cluster
					//   2. find the farthest from the center point in the biggest cluster
					//   3. exclude the farthest point from the biggest cluster and form a new 1-point cluster.
					int max_k = 0;
					for( int k1 = 1; k1 < Numcluster; k1++ )
					{
						if( counters[max_k] < counters[k1] )
							max_k = k1;
					}

					double max_dist = 0;
					int farthest_i = -1;
					float* new_center  = centers.ptr<float>(k);
					float* old_center  = centers.ptr<float>(max_k);
					float* _old_center = temp.ptr<float>(); // normalized
					float scale = 1.f/counters[max_k];
					for( j = 0; j < dims; j++ )
						_old_center[j] = old_center[j]*scale;

					for( i = 0; i < Num_data; i++ )
					{
						if( labels[i] != max_k )
							continue;
						sample = data.ptr<float>(i);
						double dist = Distance_Measure(sample, _old_center, dims);

						if( max_dist <= dist )
						{
							max_dist = dist;
							farthest_i = i;
						}
					}

					counters[max_k]--;
					counters[k]++;
					labels[farthest_i] = k;
					sample = data.ptr<float>(farthest_i);

					for( j = 0; j < dims; j++ )
					{
						old_center[j] -= sample[j];
						new_center[j] += sample[j];
					}
				}

				for( k = 0; k < Numcluster; k++ )
				{
					float* center = centers.ptr<float>(k);
					CV_Assert( counters[k] != 0 );

					float scale = 1.f/counters[k];
					for( j = 0; j < dims; j++ )
						center[j] *= scale;

					if( iter > 0 )
					{
						double dist = 0;
						const float* old_center = old_centers.ptr<float>(k);
						for( j = 0; j < dims; j++ )
						{
							double t = center[j] - old_center[j];
							dist += t*t;
						}
						max_center_shift = std::max(max_center_shift, dist);
					}
				}
			}

			if( ++iter == 10 || max_center_shift <= FLT_EPSILON*FLT_EPSILON )
				break;


			// assign labels change this part to flann based index 
			cv::Mat dists(1, Num_data, CV_64F);
			double* dist = dists.ptr<double>(0);
			Flann_Search(dist, labels, data, centers);
			compactness = 0;
			for( i = 0; i < Num_data; i++ )
			{
				compactness += dist[i];
			}
		}

		if( compactness < best_compactness )
		{
			best_compactness = compactness;
			if( _centers.needed() )
				centers.copyTo(_centers);
			_labels.copyTo(best_labels);
		}
	}

	return best_compactness;
}

BOW_Trainer::BOW_Trainer()
{}

BOW_Trainer::~BOW_Trainer()
{}

void BOW_Trainer::addDes( const cv::Mat& _descriptors )
{
	CV_Assert( !_descriptors.empty() );
	if( !descriptors.empty() )
	{
		CV_Assert( descriptors[0].cols == _descriptors.cols );
		CV_Assert( descriptors[0].type() == _descriptors.type() );
		size += _descriptors.rows;
	}
	else
	{
		size = _descriptors.rows;
	}
	descriptors.push_back(_descriptors);
}

const vector<cv::Mat>& BOW_Trainer::getDescriptors() const
{
	return descriptors;
}

int BOW_Trainer::descripotorsCount() const
{
	return descriptors.empty() ? 0 : size;
}

void BOW_Trainer::clear()
{
	descriptors.clear();
}

BOW_KMeansTrainer::BOW_KMeansTrainer( int _clusterCount) :
clusterCount(_clusterCount)
{}

cv::Mat BOW_KMeansTrainer::cluster() const
{
	CV_Assert( !descriptors.empty() );

	int descCount = 0;
	for( size_t i = 0; i < descriptors.size(); i++ )
		descCount += descriptors[i].rows;

	cv::Mat mergedDescriptors( descCount, descriptors[0].cols, descriptors[0].type() );
	for( size_t i = 0, start = 0; i < descriptors.size(); i++ )
	{
		cv::Mat submut = mergedDescriptors.rowRange((int)start, (int)(start + descriptors[i].rows));
		descriptors[i].copyTo(submut);
		start += descriptors[i].rows;
	}
	printf("begin the cluster process!\n");
	return cluster( mergedDescriptors );
}

BOW_KMeansTrainer::~BOW_KMeansTrainer()
{}

cv::Mat BOW_KMeansTrainer::cluster( const cv::Mat& _descriptors ) const
{
	cv::Mat vocabulary;
	double  compactness=Kmeans_Cluster( _descriptors, clusterCount, vocabulary );
	cout<<compactness<<endl;
	return vocabulary;
}

BOW_DescriptorExtractor::BOW_DescriptorExtractor( const cv::Ptr<cv::DescriptorMatcher>& _dmatcher  ) :
 dmatcher(_dmatcher)
{}

BOW_DescriptorExtractor::~BOW_DescriptorExtractor()
{}

void BOW_DescriptorExtractor::setVocabulary( const cv::Mat& _vocabulary )
{
	dmatcher->clear();
	vocabulary = _vocabulary;
	dmatcher->add( std::vector<cv::Mat>(1, vocabulary) );
}

const cv::Mat& BOW_DescriptorExtractor::getVocabulary() const
{
	return vocabulary;
}

void BOW_DescriptorExtractor::bow_compute( cv::Mat& descriptors, cv::Mat& imgDescriptor )
{
	imgDescriptor.release();
	int clusterCount = descriptorSize(); 

	// Match key point descriptors to cluster center (to vocabulary)
	std::vector<cv::DMatch> matches;
	dmatcher->match(descriptors,matches);

	if ( !descriptors.empty())
	{
		imgDescriptor = cv::Mat( 1, clusterCount, CV_32FC1, cv::Scalar::all(0.0) );
		float *dptr   = (float*)imgDescriptor.data;
		for( size_t i = 0; i < matches.size(); i++ )
		{
			int queryIdx = matches[i].queryIdx;
			int trainIdx = matches[i].trainIdx; // cluster index
		    CV_Assert( queryIdx == (int)i );

			dptr[trainIdx] = dptr[trainIdx] + 1.f;		
		}
	} 
	else
	{
		for ( int j=0; j<clusterCount; j++ )
		{
			imgDescriptor = cv::Mat( 1, clusterCount, CV_32FC1, cv::Scalar::all(0.0) );
			imgDescriptor.at<float>(0,j)=FLT_MAX;
		}
	}

	// Normalize image descriptor.
	imgDescriptor /= descriptors.rows;
}

int BOW_DescriptorExtractor::descriptorSize() const
{
	return vocabulary.empty() ? 0 : vocabulary.rows;
}

int BOW_DescriptorExtractor::descriptorType() const
{
	return CV_32FC1;
}