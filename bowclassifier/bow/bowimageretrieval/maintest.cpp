#include "cvbow.h"
#include "sift.h"
#include <dirent.h>

using namespace std;

#define  DictionarySize 50        //Dictionary Size
std::string Timagefilepath        = "../data/datasets/";
std::string Timagedescriptor_path = "../data/siftfeatures/";
std::string Dictionary_path       = "../data/dictionary/";
std::string BOWdataset_path       = "../data/bowfeatures/";


//load the files from the folder in the disk
void GetFileList(std::string &filepath, std::vector<std::string> &Name_list)
{
       DIR * dir;
       struct dirent * ptr;
       char pathbuf[500];
       strcpy(pathbuf, filepath.c_str());
       dir = opendir(pathbuf);
       while((ptr = readdir(dir)) != NULL)
       {
         if(strstr(ptr->d_name, ".jpg")||strstr(ptr->d_name, ".png")||strstr(ptr->d_name,".jpeg")||strstr(ptr->d_name,".yaml"))
         {
           Name_list.push_back(ptr->d_name);
         }
       }
       closedir(dir);
}

/*step 1: 
compute the SIFT descriptors of the images in the dataset and save them to the disk*/
void Compute_Descriptors( std::string &images_path, std::string & Descriptor_path)
{
	//read images from image dataset
	cout<<"compute sift features for each image"<<endl;
	std::vector<std::string>Image_names;
	GetFileList(images_path, Image_names);
	int image_numbers=Image_names.size();

       //define the salient point detector and descriptor
	Sift_FeatureDetector detector;
	Sift_DescriptorExtractor extractor;
	
	for ( int i = 0; i < image_numbers; ++i)
	{		
		cv::Mat image = cv::imread(images_path+Image_names[i].c_str(), 1);
		cv::Mat descriptors;
		std::vector<cv::KeyPoint> keypoints;

		/*keypoints detection*/
		detector.detect(image, keypoints);

		/*descriptor extraction*/
		extractor.compute(image, keypoints, descriptors);

		std::cout << Image_names[i].c_str() << " " << keypoints.size() << endl;

		/*Write the descriptors to disk*/
		cv::FileStorage fs(Descriptor_path + Image_names[i].c_str() + ".yaml", cv::FileStorage::WRITE);
		cvWriteComment(*fs, "SIFT", 0);
		cv::write(fs, "keypoints", keypoints);
		fs << "descriptors" << descriptors;
		keypoints.clear();	
	}	
}

/*if the descriptor is calculated, then load them from disk directly and add to the bowTrainer*/
void Load_descriptors(std::string &localfeaturepath, BOW_KMeansTrainer &bowTrainer)
{
	/*read the local descriptors*/
    std::vector<std::string>File_names;
	GetFileList(localfeaturepath, File_names);
	int file_numbers=File_names.size();	

	/*add the descriptors to bowTrainer*/

	int descriptor_number = 0;
	
	for (int i = 0; i < file_numbers; ++i)
	{
		cv::FileStorage fs(localfeaturepath+File_names[i].c_str(), cv::FileStorage::READ);
		cv::Mat local_descriptor;
		fs["descriptors"] >> local_descriptor;		
		bowTrainer.addDes(local_descriptor);
		descriptor_number += local_descriptor.rows;
	}	
	cout << descriptor_number << "descriptors read" << endl;	
}


/*step 2: 
train the dictionary, use the fast kmeans cluster, which is based on the flann search*/ 
void Save_dictionary(const std::string & Dictionary_path, cv::Mat& Dictionary)
{
	cv::FileStorage fs(Dictionary_path + "dictionary.yaml", cv::FileStorage::WRITE);
	cvWriteComment(*fs, "DICTIONARY", 0);
	cv::write(fs, "dictionary", Dictionary);
}
//train the dictionary 
void Creat_Dictionary(std::string &localfeature_path, cv::Mat &Dictionary)
{
	cout<<"train the dictionary"<<endl;
	BOW_KMeansTrainer bowTrainer(DictionarySize);
			
	/*load the descriptors from the disk and add them to the bowtrainer*/
	Load_descriptors( localfeature_path, bowTrainer );

	Dictionary=bowTrainer.cluster();

	Save_dictionary(Dictionary_path,Dictionary);
}

//load the dictionary from disk 
void Load_Dictionary(cv::Mat &vocabulary)
{
	cv::FileStorage fs(Dictionary_path + "dictionary.yaml",cv::FileStorage::READ);
	fs["dictionary"]>>vocabulary;
	cout<<"Dictionary Loaded"<<endl;
}

/*step 3: 
generated the bag of word histogram */
//compute the bow feature for all images in a dataset
void Bowdescriptor_Compute_Dataset(std::string Localfeatures_path, std::string &bowfeature_path, cv::Mat& Dictionary)
{
	cout<<"compute the bow feature for each image"<<endl;
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");/*define the matcher for real value features*/
	BOW_DescriptorExtractor BOWExtractor(matcher);/*define BOWExtractor*/	

	BOWExtractor.setVocabulary(Dictionary);//build the index for the dictionary mat

	/*read the local descriptors*/
    std::vector<std::string>File_names;
	GetFileList(Localfeatures_path, File_names);
	int file_numbers=File_names.size();	

	for ( int i=0; i<file_numbers; ++i )
	{
		cv::FileStorage fs(Localfeatures_path + File_names[i].c_str(),cv::FileStorage::READ);
		cv::Mat Local_descriptor;
		cv::Mat Bow_descriptor;
		fs["descriptors"]>>Local_descriptor;
        BOWExtractor.bow_compute(Local_descriptor,Bow_descriptor);
		
		/*Write the bow to disk*/
		cv::FileStorage fs1(bowfeature_path + File_names[i].c_str() + ".yaml", cv::FileStorage::WRITE);
		cvWriteComment(*fs1,"BOW",0);
		fs1 << "Bow_descriptor" << Bow_descriptor;		
	}
}

//Compute the bow features for one query 
cv::Mat Bowdescriptor_Compute_Query(cv::Mat& query_image, cv::Mat& Dictionary)
{
	
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");/*define the matcher for real value features*/
	BOW_DescriptorExtractor BOWExtractor(matcher);/*define BOWExtractor*/
	BOWExtractor.setVocabulary(Dictionary);//build the index for the dictionary mat
	
	Sift_FeatureDetector detector;
	Sift_DescriptorExtractor extractor;
	cv::Mat descriptors;
	std::vector<cv::KeyPoint> keypoints;

	/*keypoints detection*/
	detector.detect(query_image, keypoints);

	/*descriptor extraction*/
	extractor.compute(query_image, keypoints, descriptors);
	cv::Mat Bow_descriptor;
	BOWExtractor.bow_compute(descriptors,Bow_descriptor);	
	return Bow_descriptor;
}


/*step 4: 
search test on a image dataset*/
void Load_asMat(std::string &bow_path,std::vector<std::string> &Namelist,cv::Mat& baseMat)
{
	/*read the descriptors*/
	std::vector<std::string>File_names;
	GetFileList(bow_path, File_names);
	int file_numbers=File_names.size();	
	/*add the BOW descriptors to Database*/

	for ( int i = 0; i < file_numbers; ++i )
	{
		cv::FileStorage fs(bow_path+File_names[i].c_str(), cv::FileStorage::READ);
		cv::Mat local_descriptor;
		fs["Bow_descriptor"] >> local_descriptor;
		
		baseMat.push_back(local_descriptor);
		Namelist.push_back(File_names[i].c_str());
	}	
}

void Query_Test(const string &queryimagepath, const string &html, std::string &datasetbowfeatures_path, cv::Mat &Vocabulary)
{
	cv::Mat queryimage=cv::imread(queryimagepath,1);
	std::vector<std::string> Database_name;
	std::vector<std::string> Query_name;
	cv::Mat Database;
	//load Bow_descriptors for query
	cv::Mat querybowfeatures=Bowdescriptor_Compute_Query(queryimage,Vocabulary);
	//load Bow_descriptors for the dataset
	Load_asMat(datasetbowfeatures_path, Database_name,Database);
	
	cv::BruteForceMatcher< cv::L2<float> > matcher;
	ofstream rank_list(html.c_str(),ios::out|ios::trunc);
	rank_list<<"<html><body><font size='4'>Query Image:</font><br>"<<endl;
	rank_list<<"<p><img src="+queryimagepath+" width="<<queryimage.cols*128/queryimage.rows<<" height=128 /><br></p>"<<endl;
	// based on k nearest neighbours match
	std::vector< std::vector<cv::DMatch> > matches;
	matcher.knnMatch(querybowfeatures,Database,matches, Database_name.size());
	//re-rank the return retrieval list
	rank_list<<"<font size='4'>Retrieval Result:</font><br>"<<endl;
	for( int n=0; n<Database_name.size(); n++ )
	{	
		int t = Database_name[matches[0][n].trainIdx].find(".yaml.yaml");		
		cv::Mat image=cv::imread(Timagefilepath+Database_name[matches[0][n].trainIdx].substr(0,t).c_str(),1);
		rank_list<<"<p><img src="<<Timagefilepath+Database_name[matches[0][n].trainIdx].substr(0,t).c_str()<<" width="<<image.cols*128/image.rows<<" height=128 /><br></p>"<<endl;
	}
}


int main(int argc, char * argv[])
{
	//step 1: compute the salient points locale descriptors
	Compute_Descriptors( Timagefilepath, Timagedescriptor_path);
	//step 2: train the Dictionary
	cv::Mat Dictionary;
	Creat_Dictionary(Timagedescriptor_path,Dictionary);
	//step 3: compute the bow feature for each image
	Load_Dictionary(Dictionary);

	Bowdescriptor_Compute_Dataset(Timagedescriptor_path,BOWdataset_path,Dictionary);
	//setp 4: do the image retrieval test, if step 1,2,3 already done, we can skip step 1,2,3
	Query_Test(argv[1],argv[2],BOWdataset_path,Dictionary);

	//note that: if we already obtained the Dictionary and Bowfeatures, just run the following codes:
	//cv::Mat Dictionary;
	//Load_Dictionary(Dictionary);
	//cout<<"Dictionary Loaded"<<endl;
	//Query_Test(argv[1],argv[2],BOWdataset_path,Dictionary);
	return 0;
}
