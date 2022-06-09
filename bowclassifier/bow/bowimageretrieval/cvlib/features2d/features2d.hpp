#ifndef __CV_FEATURES_2D_HPP__
#define __CV_FEATURES_2D_HPP__

#include "../../cvlib/core/core.hpp"
#include "../../cvlib/flann/miniflann.hpp"

#ifdef __cplusplus
#include <limits>

extern "C" {
#endif


#ifdef __cplusplus
}

namespace cv
{
    struct CV_EXPORTS DefaultRngAuto
    {
        const uint64 old_state;

        DefaultRngAuto() : old_state(theRNG().state) { theRNG().state = (uint64)-1; }
        ~DefaultRngAuto() { theRNG().state = old_state; }

        DefaultRngAuto& operator=(const DefaultRngAuto&);
    };


// CvAffinePose: defines a parameterized affine transformation of an image patch.
// An image patch is rotated on angle phi (in degrees), then scaled lambda1 times
// along horizontal and lambda2 times along vertical direction, and then rotated again
// on angle (theta - phi).
class CV_EXPORTS CvAffinePose
{
public:
    float phi;
    float theta;
    float lambda1;
    float lambda2;
};

/*!
 The Keypoint Class
 
 The class instance stores a keypoint, i.e. a point feature found by one of many available keypoint detectors, such as
 Harris corner detector, cv::FAST, cv::StarDetector, cv::SURF, cv::SIFT, cv::LDetector etc.
 
 The keypoint is characterized by the 2D position, scale
 (proportional to the diameter of the neighborhood that needs to be taken into account),
 orientation and some other parameters. The keypoint neighborhood is then analyzed by another algorithm that builds a descriptor
 (usually represented as a feature vector). The keypoints representing the same object in different images can then be matched using
 cv::KDTree or another method.
*/
class CV_EXPORTS_W_SIMPLE KeyPoint
{
public:
    //! the default constructor
    CV_WRAP KeyPoint() : pt(0,0), size(0), angle(-1), response(0), octave(0), class_id(-1) {}
    //! the full constructor
    KeyPoint(Point2f _pt, float _size, float _angle=-1,
            float _response=0, int _octave=0, int _class_id=-1)
            : pt(_pt), size(_size), angle(_angle),
            response(_response), octave(_octave), class_id(_class_id) {}
    //! another form of the full constructor
    CV_WRAP KeyPoint(float x, float y, float _size, float _angle=-1,
            float _response=0, int _octave=0, int _class_id=-1)
            : pt(x, y), size(_size), angle(_angle),
            response(_response), octave(_octave), class_id(_class_id) {}
    
    size_t hash() const;
    
    //! converts vector of keypoints to vector of points
    static void convert(const std::vector<KeyPoint>& keypoints,
                        CV_OUT std::vector<Point2f>& points2f,
                        const std::vector<int>& keypointIndexes=std::vector<int>());
    //! converts vector of points to the vector of keypoints, where each keypoint is assigned the same size and the same orientation
    static void convert(const std::vector<Point2f>& points2f,
                        CV_OUT std::vector<KeyPoint>& keypoints,
                        float size=1, float response=1, int octave=0, int class_id=-1);

    //! computes overlap for pair of keypoints;
    //! overlap is a ratio between area of keypoint regions intersection and
    //! area of keypoint regions union (now keypoint region is circle)
    static float overlap(const KeyPoint& kp1, const KeyPoint& kp2);

    CV_PROP_RW Point2f pt; //!< coordinates of the keypoints
    CV_PROP_RW float size; //!< diameter of the meaningful keypoint neighborhood
    CV_PROP_RW float angle; //!< computed orientation of the keypoint (-1 if not applicable)
    CV_PROP_RW float response; //!< the response by which the most strong keypoints have been selected. Can be used for the further sorting or subsampling
    CV_PROP_RW int octave; //!< octave (pyramid layer) from which the keypoint has been extracted
    CV_PROP_RW int class_id; //!< object class (if the keypoints need to be clustered by an object they belong to) 
};
    
//! writes vector of keypoints to the file storage
CV_EXPORTS void write(FileStorage& fs, const string& name, const vector<KeyPoint>& keypoints);
//! reads vector of keypoints from the specified file storage node
CV_EXPORTS void read(const FileNode& node, CV_OUT vector<KeyPoint>& keypoints);    

/*
 * A class filters a vector of keypoints.
 * Because now it is difficult to provide a convenient interface for all usage scenarios of the keypoints filter class,
 * it has only 4 needed by now static methods.
 */
class CV_EXPORTS KeyPointsFilter
{
public:
    KeyPointsFilter(){}

    /*
     * Remove keypoints within borderPixels of an image edge.
     */
    static void runByImageBorder( vector<KeyPoint>& keypoints, Size imageSize, int borderSize );
    /*
     * Remove keypoints of sizes out of range.
     */
    static void runByKeypointSize( vector<KeyPoint>& keypoints, float minSize, float maxSize=std::numeric_limits<float>::max() );
    /*
     * Remove keypoints from some image by mask for pixels of this image.
     */
    static void runByPixelsMask( vector<KeyPoint>& keypoints, const Mat& mask );
    /*
     * Remove duplicated keypoints.
     */
    static void removeDuplicated( vector<KeyPoint>& keypoints );
};

/*!
 The Patch Generator class 
*/
class CV_EXPORTS PatchGenerator
{
public:
    PatchGenerator();
    PatchGenerator(double _backgroundMin, double _backgroundMax,
                   double _noiseRange, bool _randomBlur=true,
                   double _lambdaMin=0.6, double _lambdaMax=1.5,
                   double _thetaMin=-CV_PI, double _thetaMax=CV_PI,
                   double _phiMin=-CV_PI, double _phiMax=CV_PI );
    void operator()(const Mat& image, Point2f pt, Mat& patch, Size patchSize, RNG& rng) const;
    void operator()(const Mat& image, const Mat& transform, Mat& patch,
                    Size patchSize, RNG& rng) const;
    void warpWholeImage(const Mat& image, Mat& matT, Mat& buf,
                        CV_OUT Mat& warped, int border, RNG& rng) const;
    void generateRandomTransform(Point2f srcCenter, Point2f dstCenter,
                                 CV_OUT Mat& transform, RNG& rng,
                                 bool inverse=false) const;
    void setAffineParam(double lambda, double theta, double phi);
    
    double backgroundMin, backgroundMax;
    double noiseRange;
    bool randomBlur;
    double lambdaMin, lambdaMax;
    double thetaMin, thetaMax;
    double phiMin, phiMax;
};


class CV_EXPORTS LDetector
{
public:
    LDetector();
    LDetector(int _radius, int _threshold, int _nOctaves,
              int _nViews, double _baseFeatureSize, double _clusteringDistance);
    void operator()(const Mat& image,
                    CV_OUT vector<KeyPoint>& keypoints,
                    int maxCount=0, bool scaleCoords=true) const;
    void operator()(const vector<Mat>& pyr,
                    CV_OUT vector<KeyPoint>& keypoints,
                    int maxCount=0, bool scaleCoords=true) const;
    void getMostStable2D(const Mat& image, CV_OUT vector<KeyPoint>& keypoints,
                         int maxCount, const PatchGenerator& patchGenerator) const;
    void setVerbose(bool verbose);
    
    void read(const FileNode& node);
    void write(FileStorage& fs, const String& name=String()) const;
    
    int radius;
    int threshold;
    int nOctaves;
    int nViews;
    bool verbose;
    
    double baseFeatureSize;
    double clusteringDistance;
};

typedef LDetector YAPE;

class CV_EXPORTS FernClassifier
{
public:
    FernClassifier();
    FernClassifier(const FileNode& node);
    FernClassifier(const vector<vector<Point2f> >& points,
                   const vector<Mat>& refimgs,
                   const vector<vector<int> >& labels=vector<vector<int> >(),
                   int _nclasses=0, int _patchSize=PATCH_SIZE,
                   int _signatureSize=DEFAULT_SIGNATURE_SIZE,
                   int _nstructs=DEFAULT_STRUCTS,
                   int _structSize=DEFAULT_STRUCT_SIZE,
                   int _nviews=DEFAULT_VIEWS,
                   int _compressionMethod=COMPRESSION_NONE,
                   const PatchGenerator& patchGenerator=PatchGenerator());
    virtual ~FernClassifier();
    virtual void read(const FileNode& n);
    virtual void write(FileStorage& fs, const String& name=String()) const;
    virtual void trainFromSingleView(const Mat& image,
                                     const vector<KeyPoint>& keypoints,
                                     int _patchSize=PATCH_SIZE,
                                     int _signatureSize=DEFAULT_SIGNATURE_SIZE,
                                     int _nstructs=DEFAULT_STRUCTS,
                                     int _structSize=DEFAULT_STRUCT_SIZE,
                                     int _nviews=DEFAULT_VIEWS,
                                     int _compressionMethod=COMPRESSION_NONE,
                                     const PatchGenerator& patchGenerator=PatchGenerator());
    virtual void train(const vector<vector<Point2f> >& points,
                       const vector<Mat>& refimgs,
                       const vector<vector<int> >& labels=vector<vector<int> >(),
                       int _nclasses=0, int _patchSize=PATCH_SIZE,
                       int _signatureSize=DEFAULT_SIGNATURE_SIZE,
                       int _nstructs=DEFAULT_STRUCTS,
                       int _structSize=DEFAULT_STRUCT_SIZE,
                       int _nviews=DEFAULT_VIEWS,
                       int _compressionMethod=COMPRESSION_NONE,
                       const PatchGenerator& patchGenerator=PatchGenerator());
    virtual int operator()(const Mat& img, Point2f kpt, vector<float>& signature) const;
    virtual int operator()(const Mat& patch, vector<float>& signature) const;
    virtual void clear();
    virtual bool empty() const;
    void setVerbose(bool verbose);
    
    int getClassCount() const;
    int getStructCount() const;
    int getStructSize() const;
    int getSignatureSize() const;
    int getCompressionMethod() const;
    Size getPatchSize() const;
    
    struct Feature
    {
        uchar x1, y1, x2, y2;
        Feature() : x1(0), y1(0), x2(0), y2(0) {}
        Feature(int _x1, int _y1, int _x2, int _y2)
        : x1((uchar)_x1), y1((uchar)_y1), x2((uchar)_x2), y2((uchar)_y2)
        {}
        template<typename _Tp> bool operator ()(const Mat_<_Tp>& patch) const
        { return patch(y1,x1) > patch(y2, x2); }
    };
    
    enum
    {
        PATCH_SIZE = 31,
        DEFAULT_STRUCTS = 50,
        DEFAULT_STRUCT_SIZE = 9,
        DEFAULT_VIEWS = 5000,
        DEFAULT_SIGNATURE_SIZE = 176,
        COMPRESSION_NONE = 0,
        COMPRESSION_RANDOM_PROJ = 1,
        COMPRESSION_PCA = 2,
        DEFAULT_COMPRESSION_METHOD = COMPRESSION_NONE
    };
    
protected:
    virtual void prepare(int _nclasses, int _patchSize, int _signatureSize,
                         int _nstructs, int _structSize,
                         int _nviews, int _compressionMethod);
    virtual void finalize(RNG& rng);
    virtual int getLeaf(int fidx, const Mat& patch) const;
    
    bool verbose;
    int nstructs;
    int structSize;
    int nclasses;
    int signatureSize;
    int compressionMethod;
    int leavesPerStruct;
    Size patchSize;
    vector<Feature> features;
    vector<int> classCounters;
    vector<float> posteriors;
};


/****************************************************************************************\
*                                 Calonder Classifier                                    *
\****************************************************************************************/

struct RTreeNode;

struct CV_EXPORTS BaseKeypoint
{
  int x;
  int y;
  IplImage* image;

  BaseKeypoint()
    : x(0), y(0), image(NULL)
  {}

  BaseKeypoint(int x, int y, IplImage* image)
    : x(x), y(y), image(image)
  {}
};

class CV_EXPORTS RandomizedTree
{
public:
  friend class RTreeClassifier;

  static const uchar PATCH_SIZE = 32;
  static const int DEFAULT_DEPTH = 9;
  static const int DEFAULT_VIEWS = 5000;
  static const size_t DEFAULT_REDUCED_NUM_DIM = 176;
  static float GET_LOWER_QUANT_PERC() { return .03f; }
  static float GET_UPPER_QUANT_PERC() { return .92f; }

  RandomizedTree();
  ~RandomizedTree();

  void train(std::vector<BaseKeypoint> const& base_set, RNG &rng,
             int depth, int views, size_t reduced_num_dim, int num_quant_bits);
  void train(std::vector<BaseKeypoint> const& base_set, RNG &rng,
             PatchGenerator &make_patch, int depth, int views, size_t reduced_num_dim,
             int num_quant_bits);

  // following two funcs are EXPERIMENTAL (do not use unless you know exactly what you do)
  static void quantizeVector(float *vec, int dim, int N, float bnds[2], int clamp_mode=0);
  static void quantizeVector(float *src, int dim, int N, float bnds[2], uchar *dst);

  // patch_data must be a 32x32 array (no row padding)
  float* getPosterior(uchar* patch_data);
  const float* getPosterior(uchar* patch_data) const;
  uchar* getPosterior2(uchar* patch_data);
  const uchar* getPosterior2(uchar* patch_data) const;

  void read(const char* file_name, int num_quant_bits);
  void read(std::istream &is, int num_quant_bits);
  void write(const char* file_name) const;
  void write(std::ostream &os) const;

  int classes() { return classes_; }
  int depth() { return depth_; }

  //void setKeepFloatPosteriors(bool b) { keep_float_posteriors_ = b; }
  void discardFloatPosteriors() { freePosteriors(1); }

  inline void applyQuantization(int num_quant_bits) { makePosteriors2(num_quant_bits); }

  // debug
  void savePosteriors(std::string url, bool append=false);
  void savePosteriors2(std::string url, bool append=false);

private:
  int classes_;
  int depth_;
  int num_leaves_;
  std::vector<RTreeNode> nodes_;
  float **posteriors_;        // 16-bytes aligned posteriors
  uchar **posteriors2_;     // 16-bytes aligned posteriors
  std::vector<int> leaf_counts_;

  void createNodes(int num_nodes, RNG &rng);
  void allocPosteriorsAligned(int num_leaves, int num_classes);
  void freePosteriors(int which);    // which: 1=posteriors_, 2=posteriors2_, 3=both
  void init(int classes, int depth, RNG &rng);
  void addExample(int class_id, uchar* patch_data);
  void finalize(size_t reduced_num_dim, int num_quant_bits);
  int getIndex(uchar* patch_data) const;
  inline float* getPosteriorByIndex(int index);
  inline const float* getPosteriorByIndex(int index) const;
  inline uchar* getPosteriorByIndex2(int index);
  inline const uchar* getPosteriorByIndex2(int index) const;
  //void makeRandomMeasMatrix(float *cs_phi, PHI_DISTR_TYPE dt, size_t reduced_num_dim);
  void convertPosteriorsToChar();
  void makePosteriors2(int num_quant_bits);
  void compressLeaves(size_t reduced_num_dim);
  void estimateQuantPercForPosteriors(float perc[2]);
};


inline uchar* getData(IplImage* image)
{
  return reinterpret_cast<uchar*>(image->imageData);
}

inline float* RandomizedTree::getPosteriorByIndex(int index)
{
  return const_cast<float*>(const_cast<const RandomizedTree*>(this)->getPosteriorByIndex(index));
}

inline const float* RandomizedTree::getPosteriorByIndex(int index) const
{
  return posteriors_[index];
}

inline uchar* RandomizedTree::getPosteriorByIndex2(int index)
{
  return const_cast<uchar*>(const_cast<const RandomizedTree*>(this)->getPosteriorByIndex2(index));
}

inline const uchar* RandomizedTree::getPosteriorByIndex2(int index) const
{
  return posteriors2_[index];
}

struct CV_EXPORTS RTreeNode
{
  short offset1, offset2;

  RTreeNode() {}
  RTreeNode(uchar x1, uchar y1, uchar x2, uchar y2)
    : offset1(y1*RandomizedTree::PATCH_SIZE + x1),
      offset2(y2*RandomizedTree::PATCH_SIZE + x2)
  {}

  //! Left child on 0, right child on 1
  inline bool operator() (uchar* patch_data) const
  {
    return patch_data[offset1] > patch_data[offset2];
  }
};

class CV_EXPORTS RTreeClassifier
{
public:
  static const int DEFAULT_TREES = 48;
  static const size_t DEFAULT_NUM_QUANT_BITS = 4;

  RTreeClassifier();
  void train(std::vector<BaseKeypoint> const& base_set,
             RNG &rng,
             int num_trees = RTreeClassifier::DEFAULT_TREES,
             int depth = RandomizedTree::DEFAULT_DEPTH,
             int views = RandomizedTree::DEFAULT_VIEWS,
             size_t reduced_num_dim = RandomizedTree::DEFAULT_REDUCED_NUM_DIM,
             int num_quant_bits = DEFAULT_NUM_QUANT_BITS);
  void train(std::vector<BaseKeypoint> const& base_set,
             RNG &rng,
             PatchGenerator &make_patch,
             int num_trees = RTreeClassifier::DEFAULT_TREES,
             int depth = RandomizedTree::DEFAULT_DEPTH,
             int views = RandomizedTree::DEFAULT_VIEWS,
             size_t reduced_num_dim = RandomizedTree::DEFAULT_REDUCED_NUM_DIM,
             int num_quant_bits = DEFAULT_NUM_QUANT_BITS);

  // sig must point to a memory block of at least classes()*sizeof(float|uchar) bytes
  void getSignature(IplImage *patch, uchar *sig) const;
  void getSignature(IplImage *patch, float *sig) const;
  void getSparseSignature(IplImage *patch, float *sig, float thresh) const;
  // TODO: deprecated in favor of getSignature overload, remove
  void getFloatSignature(IplImage *patch, float *sig) const { getSignature(patch, sig); }

  static int countNonZeroElements(float *vec, int n, double tol=1e-10);
  static inline void safeSignatureAlloc(uchar **sig, int num_sig=1, int sig_len=176);
  static inline uchar* safeSignatureAlloc(int num_sig=1, int sig_len=176);

  inline int classes() const { return classes_; }
  inline int original_num_classes() const { return original_num_classes_; }

  void setQuantization(int num_quant_bits);
  void discardFloatPosteriors();

  void read(const char* file_name);
  void read(std::istream &is);
  void write(const char* file_name) const;
  void write(std::ostream &os) const;

  // experimental and debug
  void saveAllFloatPosteriors(std::string file_url);
  void saveAllBytePosteriors(std::string file_url);
  void setFloatPosteriorsFromTextfile_176(std::string url);
  float countZeroElements();

  std::vector<RandomizedTree> trees_;

private:
  int classes_;
  int num_quant_bits_;
  mutable uchar **posteriors_;
  mutable unsigned short *ptemp_;
  int original_num_classes_;
  bool keep_floats_;
};



/****************************************************************************************\
*                                    FeatureDetector                                     *
\****************************************************************************************/
class CV_EXPORTS FeatureDetector
{
public:
    virtual ~FeatureDetector();
    
    /*
     * Detect keypoints in an image.
     * image        The image.
     * keypoints    The detected keypoints.
     * mask         Mask specifying where to look for keypoints (optional). Must be a char
     *              matrix with non-zero values in the region of interest.
     */
    void detect( const Mat& image, vector<KeyPoint>& keypoints, const Mat& mask=Mat() ) const;
    
    /*
     * Detect keypoints in an image set.
     * images       Image collection.
     * keypoints    Collection of keypoints detected in an input images. keypoints[i] is a set of keypoints detected in an images[i].
     * masks        Masks for image set. masks[i] is a mask for images[i].
     */
    void detect( const vector<Mat>& images, vector<vector<KeyPoint> >& keypoints, const vector<Mat>& masks=vector<Mat>() ) const;

    // Read detector object from a file node.
    virtual void read( const FileNode& );
    // Read detector object from a file node.
    virtual void write( FileStorage& ) const;

    // Return true if detector object is empty
    virtual bool empty() const;

    // Create feature detector by detector name.
    static Ptr<FeatureDetector> create( const string& detectorType );

protected:
    virtual void detectImpl( const Mat& image, vector<KeyPoint>& keypoints, const Mat& mask=Mat() ) const = 0;

    /*
     * Remove keypoints that are not in the mask.
     * Helper function, useful when wrapping a library call for keypoint detection that
     * does not support a mask argument.
     */
    static void removeInvalidPoints( const Mat& mask, vector<KeyPoint>& keypoints );
};



/****************************************************************************************\
*                                 DescriptorExtractor                                    *
\****************************************************************************************/

/*
 * Abstract base class for computing descriptors for image keypoints.
 *
 * In this interface we assume a keypoint descriptor can be represented as a
 * dense, fixed-dimensional vector of some basic type. Most descriptors used
 * in practice follow this pattern, as it makes it very easy to compute
 * distances between descriptors. Therefore we represent a collection of
 * descriptors as a cv::Mat, where each row is one keypoint descriptor.
 */
class CV_EXPORTS DescriptorExtractor
{
public:
    virtual ~DescriptorExtractor();

    /*
     * Compute the descriptors for a set of keypoints in an image.
     * image        The image.
     * keypoints    The input keypoints. Keypoints for which a descriptor cannot be computed are removed.
     * descriptors  Copmputed descriptors. Row i is the descriptor for keypoint i.
     */
    void compute( const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors ) const;

    /*
     * Compute the descriptors for a keypoints collection detected in image collection.
     * images       Image collection.
     * keypoints    Input keypoints collection. keypoints[i] is keypoints detected in images[i].
     *              Keypoints for which a descriptor cannot be computed are removed.
     * descriptors  Descriptor collection. descriptors[i] are descriptors computed for set keypoints[i].
     */
    void compute( const vector<Mat>& images, vector<vector<KeyPoint> >& keypoints, vector<Mat>& descriptors ) const;

    virtual void read( const FileNode& );
    virtual void write( FileStorage& ) const;

    virtual int descriptorSize() const = 0;
    virtual int descriptorType() const = 0;

    virtual bool empty() const;

    static Ptr<DescriptorExtractor> create( const string& descriptorExtractorType );

protected:
    virtual void computeImpl( const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors ) const = 0;

    /*
     * Remove keypoints within borderPixels of an image edge.
     */
    static void removeBorderKeypoints( vector<KeyPoint>& keypoints,
                                       Size imageSize, int borderSize );
};


/****************************************************************************************\
*                                          Distance                                      *
\****************************************************************************************/
template<typename T>
struct CV_EXPORTS Accumulator
{
    typedef T Type;
};

template<> struct Accumulator<unsigned char>  { typedef float Type; };
template<> struct Accumulator<unsigned short> { typedef float Type; };
template<> struct Accumulator<char>   { typedef float Type; };
template<> struct Accumulator<short>  { typedef float Type; };

/*
 * Squared Euclidean distance functor
 */
template<class T>
struct CV_EXPORTS SL2
{
    typedef T ValueType;
    typedef typename Accumulator<T>::Type ResultType;

    ResultType operator()( const T* a, const T* b, int size ) const
    {
        ResultType result = ResultType();
        for( int i = 0; i < size; i++ )
        {
            ResultType diff = (ResultType)(a[i] - b[i]);
            result += diff*diff;
        }
        return result;
    }
};

/*
 * Euclidean distance functor
 */
template<class T>
struct CV_EXPORTS L2
{
    typedef T ValueType;
    typedef typename Accumulator<T>::Type ResultType;

    ResultType operator()( const T* a, const T* b, int size ) const
    {
        ResultType result = ResultType();
        for( int i = 0; i < size; i++ )
        {
            ResultType diff = (ResultType)(a[i] - b[i]);
            result += diff*diff;
        }
        return (ResultType)sqrt((double)result);
    }
};

/*
 * Manhattan distance (city block distance) functor
 */
template<class T>
struct CV_EXPORTS L1
{
    typedef T ValueType;
    typedef typename Accumulator<T>::Type ResultType;

    ResultType operator()( const T* a, const T* b, int size ) const
    {
        ResultType result = ResultType();
        for( int i = 0; i < size; i++ )
        {
            ResultType diff = a[i] - b[i];
            result += (ResultType)fabs( diff );
        }
        return result;
    }
};

/*
 * Hamming distance functor - counts the bit differences between two strings - useful for the Brief descriptor
 * bit count of A exclusive XOR'ed with B
 */
struct CV_EXPORTS HammingLUT
{
    typedef unsigned char ValueType;
    typedef int ResultType;

    /** this will count the bits in a ^ b
     */
    ResultType operator()( const unsigned char* a, const unsigned char* b, int size ) const;

    /** \brief given a byte, count the bits using a compile time generated look up table
     *  \param b the byte to count bits.  The look up table has an entry for all
     *  values of b, where that entry is the number of bits.
     *  \return the number of bits in byte b
     */
    static unsigned char byteBitsLookUp(unsigned char b);
};


/// Hamming distance functor, this one will try to use gcc's __builtin_popcountl
/// but will fall back on HammingLUT if not available
/// bit count of A exclusive XOR'ed with B
struct CV_EXPORTS Hamming
{
    typedef unsigned char ValueType;

    //! important that this is signed as weird behavior happens
    // in BruteForce if not
    typedef int ResultType;

    /** this will count the bits in a ^ b, using __builtin_popcountl try compiling with sse4
    */
    ResultType operator()(const unsigned char* a, const unsigned char* b, int size) const;
};


/****************************************************************************************\
*                                      DMatch                                            *
\****************************************************************************************/
/*
 * Struct for matching: query descriptor index, train descriptor index, train image index and distance between descriptors.
 */
struct CV_EXPORTS DMatch
{
    DMatch() : queryIdx(-1), trainIdx(-1), imgIdx(-1), distance(std::numeric_limits<float>::max()) {}
    DMatch( int _queryIdx, int _trainIdx, float _distance ) :
            queryIdx(_queryIdx), trainIdx(_trainIdx), imgIdx(-1), distance(_distance) {}
    DMatch( int _queryIdx, int _trainIdx, int _imgIdx, float _distance ) :
            queryIdx(_queryIdx), trainIdx(_trainIdx), imgIdx(_imgIdx), distance(_distance) {}

    int queryIdx; // query descriptor index
    int trainIdx; // train descriptor index
    int imgIdx;   // train image index

    float distance;

    // less is better
    bool operator<( const DMatch &m ) const
    {
        return distance < m.distance;
    }
};

/****************************************************************************************\
*                                  DescriptorMatcher                                     *
\****************************************************************************************/
/*
 * Abstract base class for matching two sets of descriptors.
 */
class CV_EXPORTS DescriptorMatcher
{
public:
    virtual ~DescriptorMatcher();

	/*
     * Add descriptors to train descriptor collection.
     * descriptors      Descriptors to add. Each descriptors[i] is a descriptors set from one image.
     */
    virtual void add( const vector<Mat>& descriptors );
    /*
     * Get train descriptors collection.
     */
    const vector<Mat>& getTrainDescriptors() const;
    /*
     * Clear train descriptors collection.
     */
    virtual void clear();

    /*
     * Return true if there are not train descriptors in collection.
     */
    virtual bool empty() const;
    /*
     * Return true if the matcher supports mask in match methods.
     */
    virtual bool isMaskSupported() const = 0;

    /*
     * Train matcher (e.g. train flann index).
     * In all methods to match the method train() is run every time before matching.
     * Some descriptor matchers (e.g. BruteForceMatcher) have empty implementation
     * of this method, other matchers really train their inner structures
     * (e.g. FlannBasedMatcher trains flann::Index). So nonempty implementation
     * of train() should check the class object state and do traing/retraining
     * only if the state requires that (e.g. FlannBasedMatcher trains flann::Index
     * if it has not trained yet or if new descriptors have been added to the train
     * collection).
     */
    virtual void train();
    /*
     * Group of methods to match descriptors from image pair.
     * Method train() is run in this methods.
     */
    // Find one best match for each query descriptor (if mask is empty).
    void match( const Mat& queryDescriptors, const Mat& trainDescriptors,
                vector<DMatch>& matches, const Mat& mask=Mat() ) const;
    // Find k best matches for each query descriptor (in increasing order of distances).
    // compactResult is used when mask is not empty. If compactResult is false matches
    // vector will have the same size as queryDescriptors rows. If compactResult is true
    // matches vector will not contain matches for fully masked out query descriptors.
    void knnMatch( const Mat& queryDescriptors, const Mat& trainDescriptors,
                   vector<vector<DMatch> >& matches, int k,
                   const Mat& mask=Mat(), bool compactResult=false ) const;
    // Find best matches for each query descriptor which have distance less than
    // maxDistance (in increasing order of distances).
    void radiusMatch( const Mat& queryDescriptors, const Mat& trainDescriptors,
                      vector<vector<DMatch> >& matches, float maxDistance,
                      const Mat& mask=Mat(), bool compactResult=false ) const;
    /*
     * Group of methods to match descriptors from one image to image set.
     * See description of similar methods for matching image pair above.
     */
    void match( const Mat& queryDescriptors, vector<DMatch>& matches,
                const vector<Mat>& masks=vector<Mat>() );
    void knnMatch( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, int k,
           const vector<Mat>& masks=vector<Mat>(), bool compactResult=false );
    void radiusMatch( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, float maxDistance,
                   const vector<Mat>& masks=vector<Mat>(), bool compactResult=false );

    // Reads matcher object from a file node
    virtual void read( const FileNode& );
    // Writes matcher object to a file storage
    virtual void write( FileStorage& ) const;

    // Clone the matcher. If emptyTrainData is false the method create deep copy of the object, i.e. copies
    // both parameters and train data. If emptyTrainData is true the method create object copy with current parameters
    // but with empty train data.
    virtual Ptr<DescriptorMatcher> clone( bool emptyTrainData=false ) const = 0;

    static Ptr<DescriptorMatcher> create( const string& descriptorMatcherType );
protected:
    /*
     * Class to work with descriptors from several images as with one merged matrix.
     * It is used e.g. in FlannBasedMatcher.
     */
    class CV_EXPORTS DescriptorCollection
    {
    public:
        DescriptorCollection();
        DescriptorCollection( const DescriptorCollection& collection );
        virtual ~DescriptorCollection();

        // Vector of matrices "descriptors" will be merged to one matrix "mergedDescriptors" here.
        void set( const vector<Mat>& descriptors );
        virtual void clear();

        const Mat& getDescriptors() const;
        const Mat getDescriptor( int imgIdx, int localDescIdx ) const;
        const Mat getDescriptor( int globalDescIdx ) const;
        void getLocalIdx( int globalDescIdx, int& imgIdx, int& localDescIdx ) const;

        int size() const;

    protected:
        Mat mergedDescriptors;
        vector<int> startIdxs;
    };

    // In fact the matching is implemented only by the following two methods. These methods suppose
    // that the class object has been trained already. Public match methods call these methods
    // after calling train().
    virtual void knnMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, int k,
           const vector<Mat>& masks=vector<Mat>(), bool compactResult=false ) = 0;
    virtual void radiusMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, float maxDistance,
           const vector<Mat>& masks=vector<Mat>(), bool compactResult=false ) = 0;

    static bool isPossibleMatch( const Mat& mask, int queryIdx, int trainIdx );
    static bool isMaskedOut( const vector<Mat>& masks, int queryIdx );

    static Mat clone_op( Mat m ) { return m.clone(); }
	void checkMasks( const vector<Mat>& masks, int queryDescriptorsCount ) const;

    // Collection of descriptors from train images.
    vector<Mat> trainDescCollection;
};

/*
 * Brute-force descriptor matcher.
 *
 * For each descriptor in the first set, this matcher finds the closest
 * descriptor in the second set by trying each one.
 *
 * For efficiency, BruteForceMatcher is templated on the distance metric.
 * For float descriptors, a common choice would be cv::L2<float>.
 */
template<class Distance>
class CV_EXPORTS BruteForceMatcher : public DescriptorMatcher
{
public:
    BruteForceMatcher( Distance d = Distance() ) : distance(d) {}
    virtual ~BruteForceMatcher() {}

    virtual bool isMaskSupported() const { return true; }

    virtual Ptr<DescriptorMatcher> clone( bool emptyTrainData=false ) const;

protected:
    virtual void knnMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, int k,
           const vector<Mat>& masks=vector<Mat>(), bool compactResult=false );
    virtual void radiusMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, float maxDistance,
           const vector<Mat>& masks=vector<Mat>(), bool compactResult=false );

    Distance distance;

private:
    /*
     * Next two methods are used to implement specialization.
     */
    static void commonKnnMatchImpl( BruteForceMatcher<Distance>& matcher,
                    const Mat& queryDescriptors, vector<vector<DMatch> >& matches, int k,
                    const vector<Mat>& masks, bool compactResult );
    static void commonRadiusMatchImpl( BruteForceMatcher<Distance>& matcher,
                    const Mat& queryDescriptors, vector<vector<DMatch> >& matches, float maxDistance,
                    const vector<Mat>& masks, bool compactResult );
};

template<class Distance>
Ptr<DescriptorMatcher> BruteForceMatcher<Distance>::clone( bool emptyTrainData ) const
{
    BruteForceMatcher* matcher = new BruteForceMatcher(distance);
    if( !emptyTrainData )
    {
        matcher->trainDescCollection.resize(trainDescCollection.size());
        std::transform( trainDescCollection.begin(), trainDescCollection.end(),
                        matcher->trainDescCollection.begin(), clone_op );
    }
    return matcher;
}

template<class Distance>
void BruteForceMatcher<Distance>::knnMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, int k,
                                                const vector<Mat>& masks, bool compactResult )
{
    commonKnnMatchImpl( *this, queryDescriptors, matches, k, masks, compactResult );
}

template<class Distance>
void BruteForceMatcher<Distance>::radiusMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches,
                                                   float maxDistance, const vector<Mat>& masks, bool compactResult )
{
    commonRadiusMatchImpl( *this, queryDescriptors, matches, maxDistance, masks, compactResult );
}

template<class Distance>
inline void BruteForceMatcher<Distance>::commonKnnMatchImpl( BruteForceMatcher<Distance>& matcher,
                          const Mat& queryDescriptors, vector<vector<DMatch> >& matches, int knn,
                          const vector<Mat>& masks, bool compactResult )
{
    typedef typename Distance::ValueType ValueType;
    typedef typename Distance::ResultType DistanceType;
    CV_DbgAssert( !queryDescriptors.empty() );
    CV_Assert( DataType<ValueType>::type == queryDescriptors.type() );
     
    int dimension = queryDescriptors.cols;
    matches.reserve(queryDescriptors.rows);

    size_t imgCount = matcher.trainDescCollection.size();
    vector<Mat> allDists( imgCount ); // distances between one query descriptor and all train descriptors
    for( size_t i = 0; i < imgCount; i++ )
        allDists[i] = Mat( 1, matcher.trainDescCollection[i].rows, DataType<DistanceType>::type );

    for( int qIdx = 0; qIdx < queryDescriptors.rows; qIdx++ )
    {
        if( matcher.isMaskedOut( masks, qIdx ) )
        {
            if( !compactResult ) // push empty vector
                matches.push_back( vector<DMatch>() );
        }
        else
        {
            // 1. compute distances between i-th query descriptor and all train descriptors
            for( size_t iIdx = 0; iIdx < imgCount; iIdx++ )
            {
                CV_Assert( DataType<ValueType>::type == matcher.trainDescCollection[iIdx].type() ||  matcher.trainDescCollection[iIdx].empty() );
                CV_Assert( queryDescriptors.cols == matcher.trainDescCollection[iIdx].cols ||
                           matcher.trainDescCollection[iIdx].empty() );

                const ValueType* d1 = (const ValueType*)(queryDescriptors.data + queryDescriptors.step*qIdx);
                allDists[iIdx].setTo( Scalar::all(std::numeric_limits<DistanceType>::max()) );
                for( int tIdx = 0; tIdx < matcher.trainDescCollection[iIdx].rows; tIdx++ )
                {
                    if( masks.empty() || matcher.isPossibleMatch(masks[iIdx], qIdx, tIdx) )
                    {
                        const ValueType* d2 = (const ValueType*)(matcher.trainDescCollection[iIdx].data +
                                                                 matcher.trainDescCollection[iIdx].step*tIdx);
                        allDists[iIdx].at<DistanceType>(0, tIdx) = matcher.distance(d1, d2, dimension);
                    }
                }
            }

            // 2. choose k nearest matches for query[i]
            matches.push_back( vector<DMatch>() );
            vector<vector<DMatch> >::reverse_iterator curMatches = matches.rbegin();
            for( int k = 0; k < knn; k++ )
            {
                DMatch bestMatch;
                bestMatch.distance = std::numeric_limits<float>::max();
                for( size_t iIdx = 0; iIdx < imgCount; iIdx++ )
                {
                    if( !allDists[iIdx].empty() )
                    {
                        double minVal;
                        Point minLoc;
                        minMaxLoc( allDists[iIdx], &minVal, 0, &minLoc, 0 );
                        if( minVal < bestMatch.distance )
                            bestMatch = DMatch( qIdx, minLoc.x, (int)iIdx, (float)minVal );
                    }
                }
                if( bestMatch.trainIdx == -1 )
                    break;

                allDists[bestMatch.imgIdx].at<DistanceType>(0, bestMatch.trainIdx) = std::numeric_limits<DistanceType>::max();
                curMatches->push_back( bestMatch );
            }
            //TODO should already be sorted at this point?
            std::sort( curMatches->begin(), curMatches->end() );
        }
    }
}

template<class Distance>
inline void BruteForceMatcher<Distance>::commonRadiusMatchImpl( BruteForceMatcher<Distance>& matcher,
                             const Mat& queryDescriptors, vector<vector<DMatch> >& matches, float maxDistance,
                             const vector<Mat>& masks, bool compactResult )
{
    typedef typename Distance::ValueType ValueType;
    typedef typename Distance::ResultType DistanceType;
	CV_DbgAssert( !queryDescriptors.empty() );
    CV_Assert( DataType<ValueType>::type == queryDescriptors.type() );
    
    int dimension = queryDescriptors.cols;
    matches.reserve(queryDescriptors.rows);

    size_t imgCount = matcher.trainDescCollection.size();
    for( int qIdx = 0; qIdx < queryDescriptors.rows; qIdx++ )
    {
        if( matcher.isMaskedOut( masks, qIdx ) )
        {
            if( !compactResult ) // push empty vector
                matches.push_back( vector<DMatch>() );
        }
        else
        {
            matches.push_back( vector<DMatch>() );
            vector<vector<DMatch> >::reverse_iterator curMatches = matches.rbegin();
            for( size_t iIdx = 0; iIdx < imgCount; iIdx++ )
            {
                CV_Assert( DataType<ValueType>::type == matcher.trainDescCollection[iIdx].type() ||
                           matcher.trainDescCollection[iIdx].empty() );
                CV_Assert( queryDescriptors.cols == matcher.trainDescCollection[iIdx].cols ||
						   matcher.trainDescCollection[iIdx].empty() );

                const ValueType* d1 = (const ValueType*)(queryDescriptors.data + queryDescriptors.step*qIdx);
                for( int tIdx = 0; tIdx < matcher.trainDescCollection[iIdx].rows; tIdx++ )
                {
                    if( masks.empty() || matcher.isPossibleMatch(masks[iIdx], qIdx, tIdx) )
                    {
                        const ValueType* d2 = (const ValueType*)(matcher.trainDescCollection[iIdx].data +
                                                                 matcher.trainDescCollection[iIdx].step*tIdx);
                        DistanceType d = matcher.distance(d1, d2, dimension);
                        if( d < maxDistance )
                            curMatches->push_back( DMatch( qIdx, tIdx, (int)iIdx, (float)d ) );
                    }
                }
            }
            std::sort( curMatches->begin(), curMatches->end() );
        }
    }
}

/*
 * BruteForceMatcher L2 specialization
 */
template<>
void BruteForceMatcher<L2<float> >::knnMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, int k,
                                                  const vector<Mat>& masks, bool compactResult );
template<>
void BruteForceMatcher<L2<float> >::radiusMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches,
                                                     float maxDistance, const vector<Mat>& masks, bool compactResult );

/*
 * Flann based matcher
 */
class CV_EXPORTS FlannBasedMatcher : public DescriptorMatcher
{
public:
    FlannBasedMatcher( const Ptr<flann::IndexParams>& indexParams=new flann::KDTreeIndexParams(),
                       const Ptr<flann::SearchParams>& searchParams=new flann::SearchParams() );

    virtual void add( const vector<Mat>& descriptors );
    virtual void clear();

    // Reads matcher object from a file node
    virtual void read( const FileNode& );
    // Writes matcher object to a file storage
    virtual void write( FileStorage& ) const;

    virtual void train();
    virtual bool isMaskSupported() const;
	
    virtual Ptr<DescriptorMatcher> clone( bool emptyTrainData=false ) const;

protected:
    static void convertToDMatches( const DescriptorCollection& descriptors,
                                   const Mat& indices, const Mat& distances,
                                   vector<vector<DMatch> >& matches );

    virtual void knnMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, int k,
                   const vector<Mat>& masks=vector<Mat>(), bool compactResult=false );
    virtual void radiusMatchImpl( const Mat& queryDescriptors, vector<vector<DMatch> >& matches, float maxDistance,
                   const vector<Mat>& masks=vector<Mat>(), bool compactResult=false );

    Ptr<flann::IndexParams> indexParams;
    Ptr<flann::SearchParams> searchParams;
    Ptr<flann::Index> flannIndex;

    DescriptorCollection mergedDescriptors;
    int addedDescCount;
};

/****************************************************************************************\
*                                GenericDescriptorMatcher                                *
\****************************************************************************************/
/*
 *   Abstract interface for a keypoint descriptor and matcher
 */
class GenericDescriptorMatcher;
typedef GenericDescriptorMatcher GenericDescriptorMatch;

class CV_EXPORTS GenericDescriptorMatcher
{
public:
    GenericDescriptorMatcher();
    virtual ~GenericDescriptorMatcher();

    /*
     * Add train collection: images and keypoints from them.
     * images       A set of train images.
     * ketpoints    Keypoint collection that have been detected on train images.
     *
     * Keypoints for which a descriptor cannot be computed are removed. Such keypoints
     * must be filtered in this method befor adding keypoints to train collection "trainPointCollection".
     * If inheritor class need perform such prefiltering the method add() must be overloaded.
     * In the other class methods programmer has access to the train keypoints by a constant link.
     */
    virtual void add( const vector<Mat>& images,
                      vector<vector<KeyPoint> >& keypoints );

    const vector<Mat>& getTrainImages() const;
    const vector<vector<KeyPoint> >& getTrainKeypoints() const;

    /*
     * Clear images and keypoints storing in train collection.
     */
    virtual void clear();
    /*
     * Returns true if matcher supports mask to match descriptors.
     */
    virtual bool isMaskSupported() = 0;
    /*
     * Train some inner structures (e.g. flann index or decision trees).
     * train() methods is run every time in matching methods. So the method implementation
     * should has a check whether these inner structures need be trained/retrained or not.
     */
    virtual void train();

    /*
     * Classifies query keypoints.
     * queryImage    The query image
     * queryKeypoints   Keypoints from the query image
     * trainImage    The train image
     * trainKeypoints   Keypoints from the train image
     */
    // Classify keypoints from query image under one train image.
    void classify( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                           const Mat& trainImage, vector<KeyPoint>& trainKeypoints ) const;
    // Classify keypoints from query image under train image collection.
    void classify( const Mat& queryImage, vector<KeyPoint>& queryKeypoints );

    /*
     * Group of methods to match keypoints from image pair.
     * Keypoints for which a descriptor cannot be computed are removed.
     * train() method is called here.
     */
    // Find one best match for each query descriptor (if mask is empty).
    void match( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                const Mat& trainImage, vector<KeyPoint>& trainKeypoints,
                vector<DMatch>& matches, const Mat& mask=Mat() ) const;
    // Find k best matches for each query keypoint (in increasing order of distances).
    // compactResult is used when mask is not empty. If compactResult is false matches
    // vector will have the same size as queryDescriptors rows.
    // If compactResult is true matches vector will not contain matches for fully masked out query descriptors.
    void knnMatch( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                   const Mat& trainImage, vector<KeyPoint>& trainKeypoints,
                   vector<vector<DMatch> >& matches, int k,
                   const Mat& mask=Mat(), bool compactResult=false ) const;
    // Find best matches for each query descriptor which have distance less than maxDistance (in increasing order of distances).
    void radiusMatch( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                      const Mat& trainImage, vector<KeyPoint>& trainKeypoints,
                      vector<vector<DMatch> >& matches, float maxDistance,
                      const Mat& mask=Mat(), bool compactResult=false ) const;
    /*
     * Group of methods to match keypoints from one image to image set.
     * See description of similar methods for matching image pair above.
     */
    void match( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                vector<DMatch>& matches, const vector<Mat>& masks=vector<Mat>() );
    void knnMatch( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                   vector<vector<DMatch> >& matches, int k,
                   const vector<Mat>& masks=vector<Mat>(), bool compactResult=false );
    void radiusMatch( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                      vector<vector<DMatch> >& matches, float maxDistance,
                      const vector<Mat>& masks=vector<Mat>(), bool compactResult=false );

    // Reads matcher object from a file node
    virtual void read( const FileNode& );
    // Writes matcher object to a file storage
    virtual void write( FileStorage& ) const;

    // Return true if matching object is empty (e.g. feature detector or descriptor matcher are empty)
    virtual bool empty() const;

    // Clone the matcher. If emptyTrainData is false the method create deep copy of the object, i.e. copies
    // both parameters and train data. If emptyTrainData is true the method create object copy with current parameters
    // but with empty train data.
    virtual Ptr<GenericDescriptorMatcher> clone( bool emptyTrainData=false ) const = 0;

    static Ptr<GenericDescriptorMatcher> create( const string& genericDescritptorMatcherType,
                                                 const string &paramsFilename=string() );

protected:
    // In fact the matching is implemented only by the following two methods. These methods suppose
    // that the class object has been trained already. Public match methods call these methods
    // after calling train().
    virtual void knnMatchImpl( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                               vector<vector<DMatch> >& matches, int k,
                               const vector<Mat>& masks, bool compactResult ) = 0;
    virtual void radiusMatchImpl( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                                  vector<vector<DMatch> >& matches, float maxDistance,
                                  const vector<Mat>& masks, bool compactResult ) = 0;
    /*
     * A storage for sets of keypoints together with corresponding images and class IDs
     */
    class CV_EXPORTS KeyPointCollection
    {
    public:
        KeyPointCollection();
        KeyPointCollection( const KeyPointCollection& collection );
        void add( const vector<Mat>& images, const vector<vector<KeyPoint> >& keypoints );
        void clear();

        // Returns the total number of keypoints in the collection
        size_t keypointCount() const;
        size_t imageCount() const;

        const vector<vector<KeyPoint> >& getKeypoints() const;
        const vector<KeyPoint>& getKeypoints( int imgIdx ) const;
        const KeyPoint& getKeyPoint( int imgIdx, int localPointIdx ) const;
        const KeyPoint& getKeyPoint( int globalPointIdx ) const;
        void getLocalIdx( int globalPointIdx, int& imgIdx, int& localPointIdx ) const;

        const vector<Mat>& getImages() const;
        const Mat& getImage( int imgIdx ) const;

    protected:
        int pointCount;

        vector<Mat> images;
        vector<vector<KeyPoint> > keypoints;
        // global indices of the first points in each image, startIndices.size() = keypoints.size()
        vector<int> startIndices;

    private:
        static Mat clone_op( Mat m ) { return m.clone(); }
    };

    KeyPointCollection trainPointCollection;
};


/*
 *  FernDescriptorMatcher
 */
class FernDescriptorMatcher;
typedef FernDescriptorMatcher FernDescriptorMatch;

class CV_EXPORTS FernDescriptorMatcher : public GenericDescriptorMatcher
{
public:
    class CV_EXPORTS Params
    {
    public:
        Params( int nclasses=0,
                int patchSize=FernClassifier::PATCH_SIZE,
                int signatureSize=FernClassifier::DEFAULT_SIGNATURE_SIZE,
                int nstructs=FernClassifier::DEFAULT_STRUCTS,
                int structSize=FernClassifier::DEFAULT_STRUCT_SIZE,
                int nviews=FernClassifier::DEFAULT_VIEWS,
                int compressionMethod=FernClassifier::COMPRESSION_NONE,
                const PatchGenerator& patchGenerator=PatchGenerator() );

        Params( const string& filename );

        int nclasses;
        int patchSize;
        int signatureSize;
        int nstructs;
        int structSize;
        int nviews;
        int compressionMethod;
        PatchGenerator patchGenerator;

        string filename;
    };

    FernDescriptorMatcher( const Params& params=Params() );
    virtual ~FernDescriptorMatcher();

    virtual void clear();

    virtual void train();

    virtual bool isMaskSupported();

    virtual void read( const FileNode &fn );
    virtual void write( FileStorage& fs ) const;
    virtual bool empty() const;
    
    virtual Ptr<GenericDescriptorMatcher> clone( bool emptyTrainData=false ) const;

protected:
    virtual void knnMatchImpl( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                               vector<vector<DMatch> >& matches, int k,
                               const vector<Mat>& masks, bool compactResult );
    virtual void radiusMatchImpl( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                                  vector<vector<DMatch> >& matches, float maxDistance,
                                  const vector<Mat>& masks, bool compactResult );

    void trainFernClassifier();
    void calcBestProbAndMatchIdx( const Mat& image, const Point2f& pt,
                                  float& bestProb, int& bestMatchIdx, vector<float>& signature );
    Ptr<FernClassifier> classifier;
    Params params;
    int prevTrainCount;
};

/****************************************************************************************\
*                                VectorDescriptorMatcher                                 *
\****************************************************************************************/

/*
 *  A class used for matching descriptors that can be described as vectors in a finite-dimensional space
 */
class VectorDescriptorMatcher;
typedef VectorDescriptorMatcher VectorDescriptorMatch;

class CV_EXPORTS VectorDescriptorMatcher : public GenericDescriptorMatcher
{
public:
    VectorDescriptorMatcher( const Ptr<DescriptorExtractor>& extractor, const Ptr<DescriptorMatcher>& matcher );
    virtual ~VectorDescriptorMatcher();

    virtual void add( const vector<Mat>& imgCollection,
                      vector<vector<KeyPoint> >& pointCollection );

    virtual void clear();

    virtual void train();

    virtual bool isMaskSupported();

    virtual void read( const FileNode& fn );
    virtual void write( FileStorage& fs ) const;
    virtual bool empty() const;

    virtual Ptr<GenericDescriptorMatcher> clone( bool emptyTrainData=false ) const;

protected:
    virtual void knnMatchImpl( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                               vector<vector<DMatch> >& matches, int k,
                               const vector<Mat>& masks, bool compactResult );
    virtual void radiusMatchImpl( const Mat& queryImage, vector<KeyPoint>& queryKeypoints,
                                  vector<vector<DMatch> >& matches, float maxDistance,
                                  const vector<Mat>& masks, bool compactResult );

    Ptr<DescriptorExtractor> extractor;
    Ptr<DescriptorMatcher> matcher;
};

/****************************************************************************************\
*                                   Drawing functions                                    *
\****************************************************************************************/
struct CV_EXPORTS DrawMatchesFlags
{
    enum{ DEFAULT = 0, // Output image matrix will be created (Mat::create),
                       // i.e. existing memory of output image may be reused.
                       // Two source image, matches and single keypoints will be drawn.
                       // For each keypoint only the center point will be drawn (without
                       // the circle around keypoint with keypoint size and orientation).
          DRAW_OVER_OUTIMG = 1, // Output image matrix will not be created (Mat::create).
                                // Matches will be drawn on existing content of output image.
          NOT_DRAW_SINGLE_POINTS = 2, // Single keypoints will not be drawn.
          DRAW_RICH_KEYPOINTS = 4 // For each keypoint the circle around keypoint with keypoint size and
                                  // orientation will be drawn.
        };
};

// Draw keypoints.
CV_EXPORTS void drawKeypoints( const Mat& image, const vector<KeyPoint>& keypoints, Mat& outImage,
                               const Scalar& color=Scalar::all(-1), int flags=DrawMatchesFlags::DEFAULT );

// Draws matches of keypints from two images on output image.
CV_EXPORTS void drawMatches( const Mat& img1, const vector<KeyPoint>& keypoints1,
                             const Mat& img2, const vector<KeyPoint>& keypoints2,
                             const vector<DMatch>& matches1to2, Mat& outImg,
                             const Scalar& matchColor=Scalar::all(-1), const Scalar& singlePointColor=Scalar::all(-1),
                             const vector<char>& matchesMask=vector<char>(), int flags=DrawMatchesFlags::DEFAULT );

CV_EXPORTS void drawMatches( const Mat& img1, const vector<KeyPoint>& keypoints1,
                             const Mat& img2, const vector<KeyPoint>& keypoints2,
                             const vector<vector<DMatch> >& matches1to2, Mat& outImg,
                             const Scalar& matchColor=Scalar::all(-1), const Scalar& singlePointColor=Scalar::all(-1),
                             const vector<vector<char> >& matchesMask=vector<vector<char> >(), int flags=DrawMatchesFlags::DEFAULT );

/****************************************************************************************\
*   Functions to evaluate the feature detectors and [generic] descriptor extractors      *
\****************************************************************************************/

CV_EXPORTS void evaluateFeatureDetector( const Mat& img1, const Mat& img2, const Mat& H1to2,
                                         vector<KeyPoint>* keypoints1, vector<KeyPoint>* keypoints2,
                                         float& repeatability, int& correspCount,
                                         const Ptr<FeatureDetector>& fdetector=Ptr<FeatureDetector>() );

CV_EXPORTS void computeRecallPrecisionCurve( const vector<vector<DMatch> >& matches1to2,
                                             const vector<vector<uchar> >& correctMatches1to2Mask,
                                             vector<Point2f>& recallPrecisionCurve );

CV_EXPORTS float getRecall( const vector<Point2f>& recallPrecisionCurve, float l_precision );
CV_EXPORTS int getNearestPoint( const vector<Point2f>& recallPrecisionCurve, float l_precision );

CV_EXPORTS void evaluateGenericDescriptorMatcher( const Mat& img1, const Mat& img2, const Mat& H1to2,
                                                  vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
                                                  vector<vector<DMatch> >* matches1to2, vector<vector<uchar> >* correctMatches1to2Mask,
                                                  vector<Point2f>& recallPrecisionCurve,
                                                  const Ptr<GenericDescriptorMatcher>& dmatch=Ptr<GenericDescriptorMatcher>() );



} /* namespace cv */

#endif /* __cplusplus */

#endif

/* End of file. */
