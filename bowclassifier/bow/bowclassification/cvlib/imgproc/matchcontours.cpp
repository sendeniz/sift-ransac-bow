#include "precomp.hpp"

CV_IMPL  double
cvMatchShapes( const void* contour1, const void* contour2,
               int method, double /*parameter*/ )
{
    CvMoments moments;
    CvHuMoments huMoments;
    double ma[7], mb[7];
    int i, sma, smb;
    double eps = 1.e-5;
    double mmm;
    double result = 0;

    if( !contour1 || !contour2 )
        CV_Error( CV_StsNullPtr, "" );

    // calculate moments of the first shape
    cvMoments( contour1, &moments );
    cvGetHuMoments( &moments, &huMoments );

    ma[0] = huMoments.hu1;
    ma[1] = huMoments.hu2;
    ma[2] = huMoments.hu3;
    ma[3] = huMoments.hu4;
    ma[4] = huMoments.hu5;
    ma[5] = huMoments.hu6;
    ma[6] = huMoments.hu7;


    // calculate moments of the second shape
    cvMoments( contour2, &moments );
    cvGetHuMoments( &moments, &huMoments );

    mb[0] = huMoments.hu1;
    mb[1] = huMoments.hu2;
    mb[2] = huMoments.hu3;
    mb[3] = huMoments.hu4;
    mb[4] = huMoments.hu5;
    mb[5] = huMoments.hu6;
    mb[6] = huMoments.hu7;

    switch (method)
    {
    case 1:
        {
            for( i = 0; i < 7; i++ )
            {
                double ama = fabs( ma[i] );
                double amb = fabs( mb[i] );

                if( ma[i] > 0 )
                    sma = 1;
                else if( ma[i] < 0 )
                    sma = -1;
                else
                    sma = 0;
                if( mb[i] > 0 )
                    smb = 1;
                else if( mb[i] < 0 )
                    smb = -1;
                else
                    smb = 0;

                if( ama > eps && amb > eps )
                {
                    ama = 1. / (sma * log10( ama ));
                    amb = 1. / (smb * log10( amb ));
                    result += fabs( -ama + amb );
                }
            }
            break;
        }

    case 2:
        {
            for( i = 0; i < 7; i++ )
            {
                double ama = fabs( ma[i] );
                double amb = fabs( mb[i] );

                if( ma[i] > 0 )
                    sma = 1;
                else if( ma[i] < 0 )
                    sma = -1;
                else
                    sma = 0;
                if( mb[i] > 0 )
                    smb = 1;
                else if( mb[i] < 0 )
                    smb = -1;
                else
                    smb = 0;

                if( ama > eps && amb > eps )
                {
                    ama = sma * log10( ama );
                    amb = smb * log10( amb );
                    result += fabs( -ama + amb );
                }
            }
            break;
        }

    case 3:
        {
            for( i = 0; i < 7; i++ )
            {
                double ama = fabs( ma[i] );
                double amb = fabs( mb[i] );

                if( ma[i] > 0 )
                    sma = 1;
                else if( ma[i] < 0 )
                    sma = -1;
                else
                    sma = 0;
                if( mb[i] > 0 )
                    smb = 1;
                else if( mb[i] < 0 )
                    smb = -1;
                else
                    smb = 0;

                if( ama > eps && amb > eps )
                {
                    ama = sma * log10( ama );
                    amb = smb * log10( amb );
                    mmm = fabs( (ama - amb) / ama );
                    if( result < mmm )
                        result = mmm;
                }
            }
            break;
        }
    default:
        CV_Error( CV_StsBadArg, "Unknown comparison method" );
    }

    return result;
}


/* End of file. */
