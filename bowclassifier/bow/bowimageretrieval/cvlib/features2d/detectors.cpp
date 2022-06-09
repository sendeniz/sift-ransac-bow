#include "precomp.hpp"

using namespace std;

namespace cv
{
/*
 *  FeatureDetector
 */

FeatureDetector::~FeatureDetector()
{}

void FeatureDetector::detect( const Mat& image, vector<KeyPoint>& keypoints, const Mat& mask ) const
{
    keypoints.clear();

    if( image.empty() )
        return;

    CV_Assert( mask.empty() || (mask.type() == CV_8UC1 && mask.size() == image.size()) );

    detectImpl( image, keypoints, mask );
}

void FeatureDetector::detect(const vector<Mat>& imageCollection, vector<vector<KeyPoint> >& pointCollection, const vector<Mat>& masks ) const
{
    pointCollection.resize( imageCollection.size() );
    for( size_t i = 0; i < imageCollection.size(); i++ )
        detect( imageCollection[i], pointCollection[i], masks.empty() ? Mat() : masks[i] );
}

void FeatureDetector::read( const FileNode& )
{}

void FeatureDetector::write( FileStorage& ) const
{}

bool FeatureDetector::empty() const
{
    return false;
}

void FeatureDetector::removeInvalidPoints( const Mat& mask, vector<KeyPoint>& keypoints )
{
    KeyPointsFilter::runByPixelsMask( keypoints, mask );
}

Ptr<FeatureDetector> FeatureDetector::create( const string& detectorType )
{
    FeatureDetector* fd = 0;
    size_t pos = 0;

    
    
    return fd;
}



}
