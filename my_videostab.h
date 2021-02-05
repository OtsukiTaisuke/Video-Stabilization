#ifndef MY_VIDEOSTAB_H
#define MY_VIDEOSTAB_H

#include <opencv2/opencv.hpp>
#include <iostream>

//using namespace cv;
//using namespace std;

class VideoStab
{
public:
    VideoStab();
    cv::VideoCapture capture;

    cv::Mat frame2;
    cv::Mat frame1;

    int k;

    const int HORIZONTAL_BORDER_CROP = 30;

    cv::Mat smoothedMat;
    cv::Mat cumulativeMat_inv;
    cv::Mat affine;

    cv::Mat smoothedFrame;

    double dx ;
    double dy ;
    double da ;
    double ds_x ;
    double ds_y ;

    double sx ;
    double sy ;

    double scaleX ;
    double scaleY ;
    double thetha ;
    double transX ;
    double transY ;

    double diff_scaleX ;
    double diff_scaleY ;
    double diff_transX ;
    double diff_transY ;
    double diff_thetha ;

    double errscaleX ;
    double errscaleY ;
    double errthetha ;
    double errtransX ;
    double errtransY ;

    double Q_scaleX ;
    double Q_scaleY ;
    double Q_thetha ;
    double Q_transX ;
    double Q_transY ;

    double R_scaleX ;
    double R_scaleY ;
    double R_thetha ;
    double R_transX ;
    double R_transY ;

    double sum_scaleX ;
    double sum_scaleY ;
    double sum_thetha ;
    double sum_transX ;
    double sum_transY ;

    double ob_scaleX ;
    double ob_scaleY ;
    double ob_thetha ;
    double ob_transX ;
    double ob_transY ;

    std::vector <cv::Point2f> features1, features2;
    std::vector <cv::Point2f> goodFeatures1, goodFeatures2;
    std::vector <uchar> status;
    std::vector <float> err;

    cv::Mat stabilize(cv::Mat frame_1 , cv::Mat frame_2);
    void reset_param(double smoothedParam,double *ob_param);

    void Kalman_Filter(
        double ob_scaleX, double ob_scaleY,
        double ob_thetha , double ob_transX , double ob_transY,
        double *scaleX , double *scaleY ,
        double *thetha , double *transX , double *transY);

    /*
    double frame_1_scaleX;
    double frame_1_scaleY;
    double frame_1_thetha;
    double frame_1_transX;
    double frame_1_transY;

    double frame_1_errscaleX;
    double frame_1_errscaleY;
    double frame_1_errthetha;
    double frame_1_errtransX;
    double frame_1_errtransY;

    double gain_scaleX;
    double gain_scaleY;
    double gain_thetha;
    double gain_transX;
    double gain_transY;
    */

};

#endif // VIDEOSTAB_H
