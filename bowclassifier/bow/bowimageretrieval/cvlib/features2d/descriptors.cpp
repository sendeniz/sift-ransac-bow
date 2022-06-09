#include "precomp.hpp"

using namespace std;

namespace cv
{
/****************************************************************************************\
*                                 DescriptorExtractor                                    *
\****************************************************************************************/
/*
 *   DescriptorExtractor
 */
DescriptorExtractor::~DescriptorExtractor()
{}

void DescriptorExtractor::compute( const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors ) const
{    
    if( image.empty() || keypoints.empty() )
    {
        descriptors.release();
        return;
    }


    KeyPointsFilter::runByImageBorder( keypoints, image.size(), 0 );
    KeyPointsFilter::runByKeypointSize( keypoints, std::numeric_limits<float>::epsilon() );

    computeImpl( image, keypoints, descriptors );
}

void DescriptorExtractor::compute( const vector<Mat>& imageCollection, vector<vector<KeyPoint> >& pointCollection, vector<Mat>& descCollection ) const
{
    CV_Assert( imageCollection.size() == pointCollection.size() );
    descCollection.resize( imageCollection.size() );
    for( size_t i = 0; i < imageCollection.size(); i++ )
        compute( imageCollection[i], pointCollection[i], descCollection[i] );
}

void DescriptorExtractor::read( const FileNode& )
{}

void DescriptorExtractor::write( FileStorage& ) const
{}

bool DescriptorExtractor::empty() const
{
    return false;
}

void DescriptorExtractor::removeBorderKeypoints( vector<KeyPoint>& keypoints,
                                                 Size imageSize, int borderSize )
{
    KeyPointsFilter::runByImageBorder( keypoints, imageSize, borderSize );
}

Ptr<DescriptorExtractor> DescriptorExtractor::create(const string& descriptorExtractorType)
{
    DescriptorExtractor* de = 0;

    size_t pos = 0;

    
    return de;
}

}
