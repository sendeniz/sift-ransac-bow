#ifndef _SIFT_H_
#define _SIFT_H_
#include "cvlib/features2d/features2d.hpp"
#include "cvlib/imgproc/imgproc.hpp"
#include "cvlib/imgproc/imgproc_c.h"
#include "cvlib/core/internal.hpp"


class  SIFT_SALIENT_METHOD
{
public:
	struct CommonParams
	{
		static const int DEFAULT_NOCTAVES = 4;
		static const int DEFAULT_NOCTAVE_LAYERS = 3;
		static const int DEFAULT_FIRST_OCTAVE = -1;
		enum { FIRST_ANGLE = 0, AVERAGE_ANGLE = 1 };

		CommonParams();
		CommonParams( int _nOctaves, int _nOctaveLayers, int /*_firstOctave*/, int /*_angleMode*/ );
		CommonParams( int _nOctaves, int _nOctaveLayers );
		int nOctaves, nOctaveLayers;
		int firstOctave; // it is not used now (firstOctave == 0 always)
		int angleMode;   // it is not used now
	};

	struct DetectorParams
	{
		static double GET_DEFAULT_THRESHOLD() { return 0.04; }
		static double GET_DEFAULT_EDGE_THRESHOLD() { return 10.0; }

		DetectorParams();
		DetectorParams( double _threshold, double _edgeThreshold );
		double threshold, edgeThreshold;
	};

	struct DescriptorParams
	{
		static double GET_DEFAULT_MAGNIFICATION() { return 3.0; }
		static const bool DEFAULT_IS_NORMALIZE = true;
		static const int DESCRIPTOR_SIZE = 128;

		DescriptorParams();
		DescriptorParams( double _magnification, bool /*_isNormalize*/, bool _recalculateAngles );
		DescriptorParams( bool _recalculateAngles );
		double magnification;
		bool isNormalize; // it is not used now (true always)
		bool recalculateAngles;
	};

	SIFT_SALIENT_METHOD();
	//! sift-detector constructor
	SIFT_SALIENT_METHOD( double _threshold, double _edgeThreshold,
		int _nOctaves=CommonParams::DEFAULT_NOCTAVES,
		int _nOctaveLayers=CommonParams::DEFAULT_NOCTAVE_LAYERS,
		int _firstOctave=CommonParams::DEFAULT_FIRST_OCTAVE,
		int _angleMode=CommonParams::FIRST_ANGLE );
	//! sift-descriptor constructor
	SIFT_SALIENT_METHOD( double _magnification, bool _isNormalize=true,
		bool _recalculateAngles = true,
		int _nOctaves=CommonParams::DEFAULT_NOCTAVES,
		int _nOctaveLayers=CommonParams::DEFAULT_NOCTAVE_LAYERS,
		int _firstOctave=CommonParams::DEFAULT_FIRST_OCTAVE,
		int _angleMode=CommonParams::FIRST_ANGLE );
	SIFT_SALIENT_METHOD( const CommonParams& _commParams,
		const DetectorParams& _detectorParams = DetectorParams(),
		const DescriptorParams& _descriptorParams = DescriptorParams() );

	//! returns the descriptor size in floats (128)
	int descriptorSize() const;
	//! finds the keypoints using SIFT algorithm
	void operator()(const cv::Mat& img, const cv::Mat& mask,
		std::vector<cv::KeyPoint>& keypoints) const;
	//! finds the keypoints and computes descriptors for them using SIFT algorithm.
	//! Optionally it can compute descriptors for the user-provided keypoints
	void operator()(const cv::Mat& img, const cv::Mat& mask,
		std::vector<cv::KeyPoint>& keypoints,
		cv::Mat& descriptors,
		bool useProvidedKeypoints=false) const;

	CommonParams getCommonParams () const;
	DetectorParams getDetectorParams () const;
	DescriptorParams getDescriptorParams () const;

protected:
	CommonParams commParams;
	DetectorParams detectorParams;
	DescriptorParams descriptorParams;
};

class Sift_FeatureDetector : public cv::FeatureDetector
{
public:
	Sift_FeatureDetector( const SIFT_SALIENT_METHOD::DetectorParams& detectorParams=SIFT_SALIENT_METHOD::DetectorParams(),
		const SIFT_SALIENT_METHOD::CommonParams& commonParams=SIFT_SALIENT_METHOD::CommonParams() );
	Sift_FeatureDetector( double threshold, double edgeThreshold,
		int nOctaves=SIFT_SALIENT_METHOD::CommonParams::DEFAULT_NOCTAVES,
		int nOctaveLayers=SIFT_SALIENT_METHOD::CommonParams::DEFAULT_NOCTAVE_LAYERS,
		int firstOctave=SIFT_SALIENT_METHOD::CommonParams::DEFAULT_FIRST_OCTAVE,
		int angleMode=SIFT_SALIENT_METHOD::CommonParams::FIRST_ANGLE );
	virtual void read( const cv::FileNode& fn );
	virtual void write( cv::FileStorage& fs ) const;

protected:
	virtual void detectImpl( const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, const cv::Mat& mask=cv::Mat() ) const;

	SIFT_SALIENT_METHOD sift;
};

/*
 * SiftDescriptorExtractor
 */
class Sift_DescriptorExtractor : public cv::DescriptorExtractor
{
public:
    Sift_DescriptorExtractor( const SIFT_SALIENT_METHOD::DescriptorParams& descriptorParams=SIFT_SALIENT_METHOD::DescriptorParams(),
                             const SIFT_SALIENT_METHOD::CommonParams& commonParams=SIFT_SALIENT_METHOD::CommonParams() );
    Sift_DescriptorExtractor( double magnification, bool isNormalize=true, bool recalculateAngles=true,
                             int nOctaves=SIFT_SALIENT_METHOD::CommonParams::DEFAULT_NOCTAVES,
                             int nOctaveLayers=SIFT_SALIENT_METHOD::CommonParams::DEFAULT_NOCTAVE_LAYERS,
                             int firstOctave=SIFT_SALIENT_METHOD::CommonParams::DEFAULT_FIRST_OCTAVE,
                             int angleMode=SIFT_SALIENT_METHOD::CommonParams::FIRST_ANGLE );

	virtual void read( const cv::FileNode &fn );
	virtual void write( cv::FileStorage &fs ) const;

    virtual int descriptorSize() const;
    virtual int descriptorType() const;

protected:
	virtual void computeImpl( const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors ) const;
    
    SIFT_SALIENT_METHOD sift;
};

#endif