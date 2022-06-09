#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include "cv.h"
#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "xform.h"
#include "utils.h"

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.5

int main(int argc, char *argv[])
{
	IplImage* img1, * img2, * img3,* stacked1, *stacked2, *stacked3, *stacked4;
        char stemp[1024];

// printf("Reading images: %s and %s\n",argv[1],argv[2]);
  if(argc != 4) {printf("\n\nUsage: getsift [image1.jpg] [image2.jpg] [image3.jpg]\n\n"); exit(0);}

	img1=read_jpeg_file(argv[1]);
	img2=read_jpeg_file(argv[2]);
	img3=read_jpeg_file(argv[3]);
	stacked1 = stack_imgs( img1, img2 );
	stacked2 = stack_imgs( img1, img2 );
	stacked3 = stack_imgs( img1, img3 );
	stacked4 = stack_imgs( img1, img3 );
	
	struct feature* feat1, * feat2, * feat3, * feat;
	struct feature** nbrs;
	struct feature** RANnb;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	int n1, n2, n3, k, i, j, m1 = 0, m2 =0, no1=0, no2=0;
	
	printf("SIFT Features Extraction: %s\n", argv[1]);
	n1 = sift_features( img1, &feat1 );
	printf("Numbers of Features from %s: %d\n",argv[1], n1);

	printf("SIFT Features Extraction: %s\n", argv[2]);
	n2 = sift_features( img2, &feat2 );
	printf("Numbers of Features from %s: %d\n",argv[2], n2);
	
	printf("SIFT Features Extraction: %s\n", argv[3]);
	n3 = sift_features( img3, &feat3 );
	printf("Numbers of Features from %s: %d\n",argv[3], n3);

        sprintf(stemp,"%s.sift.jpg",argv[1]);
	draw_keypoint(  img1,  feat1, n1 );
	write_jpeg_file(stemp,img1);

        sprintf(stemp,"%s.sift.jpg",argv[2]);
	draw_keypoint(  img2,  feat2, n2 );
	write_jpeg_file(stemp,img2);

	sprintf(stemp,"%s.sift.jpg",argv[3]);
	draw_keypoint(  img3,  feat3, n3 );
	write_jpeg_file(stemp,img3);
	
	FILE * feat1file;
	FILE * feat2file;
	FILE * feat3file;
	
	feat1file=fopen("features1.txt","w+");
	for(i=0;i<n1;i++)
	{
		fprintf(feat1file,"(%lf,%lf): {",(feat1+i)->x,(feat1+i)->y);	
		for(j=0;j<FEATURE_MAX_D;j++)
			fprintf(feat1file,"% lf ",(feat1+i)->descr[j]);
		fprintf(feat1file,"}\n");
	}
    printf("coordinate and descriptor of %s keypoints have been written in featfile1.txt\n",argv[1]);

	feat2file=fopen("features2.txt","w+");
	for(i=0;i<n2;i++)
	{
		fprintf(feat2file,"(%lf,%lf): {",(feat2+i)->x,(feat2+i)->y);
		for(j=0;j<FEATURE_MAX_D;j++)
			fprintf(feat2file,"% lf ",(feat2+i)->descr[j]);
		fprintf(feat2file,"}\n");
	}
	printf("coordinate and descriptor of %s keypoints have been written in featfile2.txt\n",argv[2]);
	
	feat3file=fopen("features3.txt","w+");
	for(i=0;i<n3;i++)
	{
		fprintf(feat3file,"(%lf,%lf): {",(feat3+i)->x,(feat3+i)->y);
		for(j=0;j<FEATURE_MAX_D;j++)
			fprintf(feat3file,"% lf ",(feat3+i)->descr[j]);
		fprintf(feat3file,"}\n");
	}
	printf("coordinate and descriptor of %s keypoints have been written in featfile3.txt\n",argv[3]);

	kd_root = kdtree_build( feat2, n2 );	
	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				pt2.y += img1->height;
				cvLine( stacked1, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
				m1++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}

	printf("Found %d total matches between img1 and img2\n", m1 );
	write_jpeg_file("matchesimg1img2.jpg",stacked1);

	kd_root = kdtree_build( feat3, n3 );	
	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				pt2.y += img1->height;
				cvLine( stacked3, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
				m2++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}
	
	printf("Found %d total matches between img1 and img3\n", m2 );
	write_jpeg_file("matchesimg1img3.jpg",stacked3);
	
	if (m1 > m2) {
	printf("Img2 is more similar to img1.");
	
	}
	else if (m1 < m2) {
	printf("Img3 is more similar to img1.");
	}
	else  {
	printf("Img1 and img2 are equally similar to img1.");
	}
    
    //CvMat* H;
    //int number=0;

    //H = ransac_xform( feat1, n1, FEATURE_FWD_MATCH, lsq_homog, 4, 0.25, homog_xfer_err, 27.0, &RANnb, &number );	

   	//for( i = 0; i < number; i++ )
	//{
	//	pt1 = cvPoint( cvRound( RANnb[i]->x ), cvRound( RANnb[i]->y ) );
	//	pt2 = cvPoint( cvRound( RANnb[i]->fwd_match->x ), cvRound( RANnb[i]->fwd_match->y ) );
	//	pt2.y += img1->height;
	//	cvLine( stacked2, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
	//	no1++;		
	//}

	//printf("Found %d total matches after RANSAC\n", no1 );
	//write_jpeg_file("matches.ransac.img1.img2.jpg",stacked2);
	
	//number = 0;
	//H = ransac_xform( feat3, n3, FEATURE_FWD_MATCH, lsq_homog, 4, 0.25, homog_xfer_err, 27.0, &RANnb, &number );	
    
    
   	//for( i = 0; i < number; i++ )
	//{
	//	pt1 = cvPoint( cvRound( RANnb[i]->x ), cvRound( RANnb[i]->y ) );
	//	pt2 = cvPoint( cvRound( RANnb[i]->fwd_match->x ), cvRound( RANnb[i]->fwd_match->y ) );
	//	pt2.y += img1->height;
	//	cvLine( stacked4, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
	//	no2++;		
	//}

	//printf("Found %d total matches between img1 and img3 after RANSAC\n", no2 );
	//write_jpeg_file("matches.ransac.img1.img3.jpg",stacked4);
	
       cvReleaseImage( &img1 );
	cvReleaseImage( &img2 );
	cvReleaseImage( &img3 );
	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );
	free( feat3 );
	return 0;
}
