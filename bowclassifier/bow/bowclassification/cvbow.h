#ifndef _CVBOW_H_
#define _CVBOW_H_

#include "cvlib/imgproc/imgproc.hpp"
#include "cvlib/highgui/highgui.hpp"
#include "cvlib/features2d/features2d.hpp"
#include "cvlib/core/core.hpp"
#include "cvlib/ml/ml.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iterator>


class BOW_Trainer
{
public:
	BOW_Trainer();
	virtual ~BOW_Trainer();

	void addDes( const cv::Mat& descriptors );
	const std::vector<cv::Mat>& getDescriptors() const;
	int descripotorsCount() const;

	virtual void clear();
	virtual cv::Mat cluster() const = 0;
	virtual cv::Mat cluster( const cv::Mat& descriptors ) const = 0;

protected:
	std::vector<cv::Mat> descriptors;
	int size;
};

class BOW_KMeansTrainer : public BOW_Trainer
{
public:
	BOW_KMeansTrainer( int clusterCount);
	virtual ~BOW_KMeansTrainer();

	// Returns trained vocabulary (i.e. cluster centers).
	virtual cv::Mat cluster() const;
	virtual cv::Mat cluster( const cv::Mat& descriptors ) const;

protected:	
	int clusterCount;	
};

/*
 * Class to compute image descriptor using bag of visual words.
 */
class BOW_DescriptorExtractor
{
public:
	BOW_DescriptorExtractor( const cv::Ptr<cv::DescriptorMatcher>& dmatcher);
    virtual ~BOW_DescriptorExtractor();

	void setVocabulary( const cv::Mat& vocabulary );
	const cv::Mat& getVocabulary() const;
	void bow_compute( cv::Mat& descriptors, cv::Mat& imgDescripto );
    
    int descriptorSize() const;
    int descriptorType() const;

protected:
	cv::Mat vocabulary;
	cv::Ptr<cv::DescriptorMatcher> dmatcher;
};

#endif
