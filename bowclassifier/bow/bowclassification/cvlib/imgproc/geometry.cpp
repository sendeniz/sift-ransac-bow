#include "precomp.hpp"


CV_IMPL CvRect
cvMaxRect( const CvRect* rect1, const CvRect* rect2 )
{
    if( rect1 && rect2 )
    {
        CvRect max_rect;
        int a, b;

        max_rect.x = a = rect1->x;
        b = rect2->x;
        if( max_rect.x > b )
            max_rect.x = b;

        max_rect.width = a += rect1->width;
        b += rect2->width;

        if( max_rect.width < b )
            max_rect.width = b;
        max_rect.width -= max_rect.x;

        max_rect.y = a = rect1->y;
        b = rect2->y;
        if( max_rect.y > b )
            max_rect.y = b;

        max_rect.height = a += rect1->height;
        b += rect2->height;

        if( max_rect.height < b )
            max_rect.height = b;
        max_rect.height -= max_rect.y;
        return max_rect;
    }
    else if( rect1 )
        return *rect1;
    else if( rect2 )
        return *rect2;
    else
        return cvRect(0,0,0,0);
}


CV_IMPL void
cvBoxPoints( CvBox2D box, CvPoint2D32f pt[4] )
{
    if( !pt )
        CV_Error( CV_StsNullPtr, "NULL vertex array pointer" );
    cv::RotatedRect(box).points((cv::Point2f*)pt);
}


int
icvIntersectLines( double x1, double dx1, double y1, double dy1,
                   double x2, double dx2, double y2, double dy2, double *t2 )
{
    double d = dx1 * dy2 - dx2 * dy1;
    int result = -1;

    if( d != 0 )
    {
        *t2 = ((x2 - x1) * dy1 - (y2 - y1) * dx1) / d;
        result = 0;
    }
    return result;
}


void
icvCreateCenterNormalLine( CvSubdiv2DEdge edge, double *_a, double *_b, double *_c )
{
    CvPoint2D32f org = cvSubdiv2DEdgeOrg( edge )->pt;
    CvPoint2D32f dst = cvSubdiv2DEdgeDst( edge )->pt;

    double a = dst.x - org.x;
    double b = dst.y - org.y;
    double c = -(a * (dst.x + org.x) + b * (dst.y + org.y));

    *_a = a + a;
    *_b = b + b;
    *_c = c;
}


void
icvIntersectLines3( double *a0, double *b0, double *c0,
                    double *a1, double *b1, double *c1, CvPoint2D32f * point )
{
    double det = a0[0] * b1[0] - a1[0] * b0[0];

    if( det != 0 )
    {
        det = 1. / det;
        point->x = (float) ((b0[0] * c1[0] - b1[0] * c0[0]) * det);
        point->y = (float) ((a1[0] * c0[0] - a0[0] * c1[0]) * det);
    }
    else
    {
        point->x = point->y = FLT_MAX;
    }
}


CV_IMPL double
cvPointPolygonTest( const CvArr* _contour, CvPoint2D32f pt, int measure_dist )
{
    double result = 0;
    
    CvSeqBlock block;
    CvContour header;
    CvSeq* contour = (CvSeq*)_contour;
    CvSeqReader reader;
    int i, total, counter = 0;
    int is_float;
    double min_dist_num = FLT_MAX, min_dist_denom = 1;
    CvPoint ip = {0,0};

    if( !CV_IS_SEQ(contour) )
    {
        contour = cvPointSeqFromMat( CV_SEQ_KIND_CURVE + CV_SEQ_FLAG_CLOSED,
                                    _contour, &header, &block );
    }
    else if( CV_IS_SEQ_POINT_SET(contour) )
    {
        if( contour->header_size == sizeof(CvContour) && !measure_dist )
        {
            CvRect r = ((CvContour*)contour)->rect;
            if( pt.x < r.x || pt.y < r.y ||
                pt.x >= r.x + r.width || pt.y >= r.y + r.height )
                return -1;
        }
    }
    else if( CV_IS_SEQ_CHAIN(contour) )
    {
        CV_Error( CV_StsBadArg,
            "Chains are not supported. Convert them to polygonal representation using cvApproxChains()" );
    }
    else
        CV_Error( CV_StsBadArg, "Input contour is neither a valid sequence nor a matrix" );

    total = contour->total;
    is_float = CV_SEQ_ELTYPE(contour) == CV_32FC2;
    cvStartReadSeq( contour, &reader, -1 );

    if( !is_float && !measure_dist && (ip.x = cvRound(pt.x)) == pt.x && (ip.y = cvRound(pt.y)) == pt.y )
    {
        // the fastest "pure integer" branch
        CvPoint v0, v;
        CV_READ_SEQ_ELEM( v, reader );

        for( i = 0; i < total; i++ )
        {
            int dist;
            v0 = v;
            CV_READ_SEQ_ELEM( v, reader );

            if( (v0.y <= ip.y && v.y <= ip.y) ||
                (v0.y > ip.y && v.y > ip.y) ||
                (v0.x < ip.x && v.x < ip.x) )
            {
                if( ip.y == v.y && (ip.x == v.x || (ip.y == v0.y &&
                    ((v0.x <= ip.x && ip.x <= v.x) || (v.x <= ip.x && ip.x <= v0.x)))) )
                    return 0;
                continue;
            }

            dist = (ip.y - v0.y)*(v.x - v0.x) - (ip.x - v0.x)*(v.y - v0.y);
            if( dist == 0 )
                return 0;
            if( v.y < v0.y )
                dist = -dist;
            counter += dist > 0;
        }

        result = counter % 2 == 0 ? -1 : 1;
    }
    else
    {
        CvPoint2D32f v0, v;
        CvPoint iv;

        if( is_float )
        {
            CV_READ_SEQ_ELEM( v, reader );
        }
        else
        {
            CV_READ_SEQ_ELEM( iv, reader );
            v = cvPointTo32f( iv );
        }

        if( !measure_dist )
        {
            for( i = 0; i < total; i++ )
            {
                double dist;
                v0 = v;
                if( is_float )
                {
                    CV_READ_SEQ_ELEM( v, reader );
                }
                else
                {
                    CV_READ_SEQ_ELEM( iv, reader );
                    v = cvPointTo32f( iv );
                }

                if( (v0.y <= pt.y && v.y <= pt.y) ||
                    (v0.y > pt.y && v.y > pt.y) ||
                    (v0.x < pt.x && v.x < pt.x) )
                {
                    if( pt.y == v.y && (pt.x == v.x || (pt.y == v0.y &&
                        ((v0.x <= pt.x && pt.x <= v.x) || (v.x <= pt.x && pt.x <= v0.x)))) )
                        return 0;
                    continue;
                }

                dist = (double)(pt.y - v0.y)*(v.x - v0.x) - (double)(pt.x - v0.x)*(v.y - v0.y);
                if( dist == 0 )
                    return 0;
                if( v.y < v0.y )
                    dist = -dist;
                counter += dist > 0;
            }

            result = counter % 2 == 0 ? -1 : 1;
        }
        else
        {
            for( i = 0; i < total; i++ )
            {
                double dx, dy, dx1, dy1, dx2, dy2, dist_num, dist_denom = 1;
        
                v0 = v;
                if( is_float )
                {
                    CV_READ_SEQ_ELEM( v, reader );
                }
                else
                {
                    CV_READ_SEQ_ELEM( iv, reader );
                    v = cvPointTo32f( iv );
                }
        
                dx = v.x - v0.x; dy = v.y - v0.y;
                dx1 = pt.x - v0.x; dy1 = pt.y - v0.y;
                dx2 = pt.x - v.x; dy2 = pt.y - v.y;
        
                if( dx1*dx + dy1*dy <= 0 )
                    dist_num = dx1*dx1 + dy1*dy1;
                else if( dx2*dx + dy2*dy >= 0 )
                    dist_num = dx2*dx2 + dy2*dy2;
                else
                {
                    dist_num = (dy1*dx - dx1*dy);
                    dist_num *= dist_num;
                    dist_denom = dx*dx + dy*dy;
                }

                if( dist_num*min_dist_denom < min_dist_num*dist_denom )
                {
                    min_dist_num = dist_num;
                    min_dist_denom = dist_denom;
                    if( min_dist_num == 0 )
                        break;
                }

                if( (v0.y <= pt.y && v.y <= pt.y) ||
                    (v0.y > pt.y && v.y > pt.y) ||
                    (v0.x < pt.x && v.x < pt.x) )
                    continue;

                dist_num = dy1*dx - dx1*dy;
                if( dy < 0 )
                    dist_num = -dist_num;
                counter += dist_num > 0;
            }

            result = sqrt(min_dist_num/min_dist_denom);
            if( counter % 2 == 0 )
                result = -result;
        }
    }

    return result;
}


/* End of file. */
