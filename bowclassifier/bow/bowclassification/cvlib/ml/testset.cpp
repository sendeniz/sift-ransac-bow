#include "precomp.hpp"

typedef struct CvDI
{
    double d;
    int    i;
} CvDI;

int CV_CDECL
icvCmpDI( const void* a, const void* b, void* )
{
    const CvDI* e1 = (const CvDI*) a;
    const CvDI* e2 = (const CvDI*) b;

    return (e1->d < e2->d) ? -1 : (e1->d > e2->d);
}

CV_IMPL void
cvCreateTestSet( int type, CvMat** samples,
                 int num_samples,
                 int num_features,
                 CvMat** responses,
                 int num_classes, ... )
{
    CvMat* mean = NULL;
    CvMat* cov = NULL;
    CvMemStorage* storage = NULL;
    
    CV_FUNCNAME( "cvCreateTestSet" );

    __BEGIN__;

    if( samples )
        *samples = NULL;
    if( responses )
        *responses = NULL;

    if( type != CV_TS_CONCENTRIC_SPHERES )
        CV_ERROR( CV_StsBadArg, "Invalid type parameter" );

    if( !samples )
        CV_ERROR( CV_StsNullPtr, "samples parameter must be not NULL" );

    if( !responses )
        CV_ERROR( CV_StsNullPtr, "responses parameter must be not NULL" );

    if( num_samples < 1 )
        CV_ERROR( CV_StsBadArg, "num_samples parameter must be positive" );

    if( num_features < 1 )
        CV_ERROR( CV_StsBadArg, "num_features parameter must be positive" );

    if( num_classes < 1 )
        CV_ERROR( CV_StsBadArg, "num_classes parameter must be positive" );

    if( type == CV_TS_CONCENTRIC_SPHERES )
    {
        CvSeqWriter writer;
        CvSeqReader reader;
        CvMat sample;
        CvDI elem;
        CvSeq* seq = NULL;
        int i, cur_class;

        CV_CALL( *samples = cvCreateMat( num_samples, num_features, CV_32FC1 ) );
        CV_CALL( *responses = cvCreateMat( 1, num_samples, CV_32SC1 ) );
        CV_CALL( mean = cvCreateMat( 1, num_features, CV_32FC1 ) );
        CV_CALL( cvSetZero( mean ) );
        CV_CALL( cov = cvCreateMat( num_features, num_features, CV_32FC1 ) );
        CV_CALL( cvSetIdentity( cov ) );

        /* fill the feature values matrix with random numbers drawn from standard
           normal distribution */
        CV_CALL( cvRandMVNormal( mean, cov, *samples ) );

        /* calculate distances from the origin to the samples and put them
           into the sequence along with indices */
        CV_CALL( storage = cvCreateMemStorage() );
        CV_CALL( cvStartWriteSeq( 0, sizeof( CvSeq ), sizeof( CvDI ), storage, &writer ));
        for( i = 0; i < (*samples)->rows; ++i )
        {
            CV_CALL( cvGetRow( *samples, &sample, i ));
            elem.i = i;
            CV_CALL( elem.d = cvNorm( &sample, NULL, CV_L2 ));
            CV_WRITE_SEQ_ELEM( elem, writer );
        }
        CV_CALL( seq = cvEndWriteSeq( &writer ) );
    
        /* sort the sequence in a distance ascending order */
        CV_CALL( cvSeqSort( seq, icvCmpDI, NULL ) );

        /* assign class labels */
        num_classes = MIN( num_samples, num_classes );
        CV_CALL( cvStartReadSeq( seq, &reader ) );
        CV_READ_SEQ_ELEM( elem, reader );
        for( i = 0, cur_class = 0; i < num_samples; ++cur_class )
        {
            int last_idx;
            double max_dst;
        
            last_idx = num_samples * (cur_class + 1) / num_classes - 1;
            CV_CALL( max_dst = (*((CvDI*) cvGetSeqElem( seq, last_idx ))).d );
            max_dst = MAX( max_dst, elem.d );

            for( ; elem.d <= max_dst && i < num_samples; ++i )
            {
                CV_MAT_ELEM( **responses, int, 0, elem.i ) = cur_class;
                if( i < num_samples - 1 )
                {
                    CV_READ_SEQ_ELEM( elem, reader );
                }
            }
        }
    }

    __END__;

    if( cvGetErrStatus() < 0 )
    {
        if( samples )
            cvReleaseMat( samples );
        if( responses )
            cvReleaseMat( responses );
    }
    cvReleaseMat( &mean );
    cvReleaseMat( &cov );
    cvReleaseMemStorage( &storage );
}

/* End of file. */
