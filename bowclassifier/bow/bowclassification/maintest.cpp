#include "cvbow.h"
#include "sift.h"
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
using namespace std;

#define  DictionarySize 15        //Dictionary Size

std::string Timagefilepath        = "../data/datasets/";
std::string Timagedescriptor_path = "../data/siftfeatures/";
std::string Dictionary_path       = "../data/dictionary/";
std::string BOWdataset_path       = "../data/bowfeatures/";
std::string svmclassifier_path    = "../data/svmclassifier/classifier.xml";

//std::string categotiesarray[10]={"accordion","soccer_ball","wild_cat"};
std::vector<std::string> categories;

void dfs_remove_dir()
{
    DIR *cur_dir = opendir(".");
    struct dirent *ent = NULL;
    struct stat st;

    if (!cur_dir)
    {
        return;
    }

    while ((ent = readdir(cur_dir)) != NULL)
    {
        stat(ent->d_name, &st);

        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        {
            continue;
        }

        if (S_ISDIR(st.st_mode))
        {
            chdir(ent->d_name);
            dfs_remove_dir();
            chdir("..");
        }

        remove(ent->d_name);
    }

    closedir(cur_dir);
}

void remove_directory(const char *path_raw)
{
    char old_path[500];

    if (!path_raw)
    {
        return;
    }

    getcwd(old_path, 500);

    if (chdir(path_raw) == -1)
    {
        return;
    }

    dfs_remove_dir();
    chdir(old_path);

}

void GetFolderListandMakefolder(std::string &imagesfilepath, std::string &localfeaturespath,std::string &bowfeaturespath)
{
   //GetFolderList
   DIR * dir;
   struct dirent * ptr;
   char pathbuf[500];
   strcpy(pathbuf, imagesfilepath.c_str());
   dir = opendir(pathbuf);
   while((ptr = readdir(dir)) != NULL)
   {
	   if(!strstr(ptr->d_name, "."))
	   {
		   categories.push_back(ptr->d_name);
		   cout<< ptr->d_name <<endl;
	   }
   }
   closedir(dir);
   //clear the Timagedescriptor_path and BOWdataset_path
    remove_directory(localfeaturespath.c_str());
    remove_directory(bowfeaturespath.c_str());
   //make folder
   for(int classid=0;classid<categories.size();classid++)
   {
	   std::string localfeatures_folder= localfeaturespath + categories[classid];
	   std::string bowfeatures_folder  = bowfeaturespath  + categories[classid];
	   int a = mkdir(localfeatures_folder.c_str(),S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
	   int b = mkdir(bowfeatures_folder.c_str(),S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
   }
}

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
void Compute_Descriptors( std::string &Timagepath, std::string &Descriptor_path)
{
	cout<<"compute sift local features for each image: "<<endl;
	for ( int classid=0; classid < categories.size(); classid++ )
	{
		//read images from image dataset
		cout<<categories[classid].c_str()<<endl;
		std::vector<std::string>Image_names;
		std::string path = Timagepath+ categories[classid] + "/";
		GetFileList(path, Image_names);
		int image_numbers=Image_names.size();
		
		//define the salient point detector and descriptor
		Sift_FeatureDetector detector;
		Sift_DescriptorExtractor extractor;
		
		for ( int i = 0; i < image_numbers; ++i)
		{
			cv::Mat image = cv::imread(Timagepath+categories[classid]+"/"+Image_names[i].c_str(), CV_LOAD_IMAGE_GRAYSCALE);
			cv::Mat descriptors;
			std::vector<cv::KeyPoint> keypoints;
			/*keypoints detection*/
			detector.detect(image, keypoints);
			/*descriptor extraction*/
			extractor.compute(image, keypoints, descriptors);
			std::cout << Image_names[i].c_str() << " " << keypoints.size() << endl;
			/*Write the descriptors to disk*/
			cv::FileStorage fs(Descriptor_path + categories[classid]+"/"+Image_names[i].c_str() + ".yaml", cv::FileStorage::WRITE);
			cvWriteComment(*fs, "SIFT", 0);
			cv::write(fs, "keypoints", keypoints);
			fs << "descriptors" << descriptors;
			keypoints.clear();
		}	
	}
}

/*if the descriptor is calculated, then load them from disk directly and add to the bowTrainer*/
void Load_descriptors(std::string &Localfeature_path, BOW_KMeansTrainer &bowTrainer)
{
	for ( int classid=0; classid< categories.size(); classid++ )
	{
		/*read the descriptors*/
		std::vector<std::string>File_names;
		std::string path=Localfeature_path+categories[classid]+"/";
		GetFileList(path, File_names);
		int file_numbers=File_names.size();	

		/*add the descriptors to bowTrainer*/
		for (int i = 0; i < file_numbers; ++i)
		{
			cv::FileStorage fs(Localfeature_path+categories[classid]+"/"+File_names[i].c_str(), cv::FileStorage::READ);
			cv::Mat local_descriptor;
			fs["descriptors"] >> local_descriptor;
			bowTrainer.addDes(local_descriptor);
		}	
	}
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
void Creat_Dictionary(cv::Mat &Dictionary)
{
	cout<<"Train the dictionary"<<endl;
	BOW_KMeansTrainer bowTrainer(DictionarySize);
			
	/*load the descriptors from the disk and add them to the bowtrainer*/
	Load_descriptors( Timagedescriptor_path, bowTrainer );

	Dictionary=bowTrainer.cluster();

	Save_dictionary(Dictionary_path,Dictionary);
}

//load the dictionary from disk 
void Load_Dictionary(cv::Mat &vocabulary)
{
	cv::FileStorage fs(Dictionary_path+"dictionary.yaml",cv::FileStorage::READ);
	fs["dictionary"]>>vocabulary;
	cout<<"Dictionary Loaded"<<endl;
}

/*step 3: 
generated the bag of word histogram */
//compute the bow feature for all images in a dataset
void Bowdescriptor_Compute_Dataset(std::string &Timagelocalfeaturepath, cv::Mat& Dictionary)
{
	cout<<"Compute the bow feature for each image"<<endl;
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");/*define the matcher for real value features*/
	BOW_DescriptorExtractor BOWExtractor(matcher);/*define BOWExtractor*/	

	BOWExtractor.setVocabulary(Dictionary);//build the index for the dictionary mat
	for ( int classid=0; classid< categories.size(); classid++ )
	{
		/*read the local descriptors*/
		std::vector<std::string>File_names;
		std::string path=Timagelocalfeaturepath+categories[classid]+"/";
		GetFileList(path, File_names);
		int file_numbers=File_names.size();
		
		for ( int i=0; i<file_numbers; ++i )
		{
			cv::FileStorage fs(Timagelocalfeaturepath+categories[classid]+"/"+File_names[i].c_str(),cv::FileStorage::READ);
			cv::Mat Local_descriptor;
			cv::Mat Bow_descriptor;
			fs["descriptors"]>>Local_descriptor;
			BOWExtractor.bow_compute(Local_descriptor,Bow_descriptor);
			
			/*Write the bow to disk*/
			cv::FileStorage fs1(BOWdataset_path +categories[classid]+"/"+ File_names[i].c_str() + ".yaml", cv::FileStorage::WRITE);
			cvWriteComment(*fs1,"BOW",0);
			fs1 << "Bow_descriptor" << Bow_descriptor;
		}
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

void SVM_Train( CvSVM &svm, std::string &TrainingDataPath)
{
	//load Bow_descriptors for the dataset
	cout<<"train the svm classifier"<<endl;
	cv::Mat TrainingData;
	cv::Mat labels(0, 1, CV_32FC1);
	for ( int classid=0; classid< categories.size(); classid++ )
	{
		std::string Filepath = TrainingDataPath + categories[classid] + "/";
		/*read the descriptors*/
		std::vector<std::string>File_names;
		GetFileList(Filepath, File_names);
		int file_numbers=File_names.size();

		/*add the BOW descriptors to baseMat*/
		for ( int i = 0; i < file_numbers; ++i )
		{
			cv::FileStorage fs(TrainingDataPath+categories[classid]+"/"+File_names[i].c_str(), cv::FileStorage::READ);
			cv::Mat local_descriptor;
			fs["Bow_descriptor"] >> local_descriptor;
			TrainingData.push_back(local_descriptor);
			labels.push_back((float)classid);
		}	
	}
	 
	//Setting up SVM parameters
	CvSVMParams params;
	params.kernel_type=CvSVM::RBF;
	params.svm_type=CvSVM::C_SVC;
	params.gamma=0.50625000000000009;
	params.C=312.50000000000000;
	params.term_crit=cvTermCriteria(CV_TERMCRIT_ITER,100,0.000001);

	bool res=svm.train(TrainingData,labels,cv::Mat(),cv::Mat(),params);
	svm.save(svmclassifier_path.c_str());
}

int Query_Predict(const string &queryimagepath,CvSVM &svm, cv::Mat &Vocabulary)
{
	cv::Mat queryimage=cv::imread(queryimagepath,1);
	//load Bow_descriptors for query
	cv::Mat querybowfeatures=Bowdescriptor_Compute_Query(queryimage,Vocabulary);
	int response = svm.predict(querybowfeatures);

	return response;

}

int main(int argc, char * argv[] )
{
	//step 1: compute the salient points locale descriptors
	GetFolderListandMakefolder(Timagefilepath,Timagedescriptor_path,BOWdataset_path);
	Compute_Descriptors(Timagefilepath,Timagedescriptor_path);
	//step 2: train the Dictionary
	cv::Mat Dictionary;
	Creat_Dictionary(Dictionary);
	//step 3: compute the bow feature for each image
	Load_Dictionary(Dictionary);

	Bowdescriptor_Compute_Dataset(Timagedescriptor_path,Dictionary);
	//step 4: train the SVM classifier
	CvSVM svm;
    SVM_Train( svm, BOWdataset_path);
	//setp 5: do the image classification test, if step 1,2,3,4 already done, we can skip step 1,2,3,4
	svm.load(svmclassifier_path.c_str());
	int ID=Query_Predict(argv[1],svm,Dictionary);
	cout<<categories[ID].c_str()<<endl;

	return 0;
}
