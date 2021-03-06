#include "my_videostab.h"
#include <cmath>

//Parameters for Kalman Filter
#define Q1 0.004
#define R1 0.5

//To see the results of before and after stabilization simultaneously
#define test false 

VideoStab::VideoStab()
{

    smoothedMat.create(2 , 3 , CV_64F);

    k = 1;

    errscaleX = 1;
    errscaleY = 1;
    errthetha = 1;
    errtransX = 1;
    errtransY = 1;

    Q_scaleX = Q1;
    Q_scaleY = Q1;
    Q_thetha = Q1;
    Q_transX = Q1;
    Q_transY = Q1;

    R_scaleX = R1;
    R_scaleY = R1;
    R_thetha = R1;
    R_transX = R1;
    R_transY = R1;

    sum_scaleX = 0;
    sum_scaleY = 0;
    sum_thetha = 0;
    sum_transX = 0;
    sum_transY = 0;

    ob_scaleX ;
    ob_scaleY ;
    ob_thetha ;
    ob_transX ;
    ob_transY ;

    scaleX = 0;
    scaleY = 0;
    thetha = 0;
    transX = 0;
    transY = 0;

}

//The main stabilization function
cv::Mat VideoStab::stabilize(cv::Mat frame_1, cv::Mat frame_2)
{
    if( !( frame_1.channels()==1 ) ){
        cv::cvtColor(frame_1, frame1, cv::COLOR_BGR2GRAY);
    }
    if( !( frame_2.channels()==1 ) ){
        cv::cvtColor(frame_2, frame2, cv::COLOR_BGR2GRAY);
    }

    int vert_border = HORIZONTAL_BORDER_CROP * frame_1.rows / frame_1.cols;
    GaussianBlur(frame2,frame2,cv::Size(5,5),4);
    GaussianBlur(frame1,frame1,cv::Size(5,5),4);

    features1.clear();
    features2.clear();
    goodFeatures1.clear();
    goodFeatures2.clear();
    status.clear();
    err.clear();

    //Estimating the features in frame1 and frame2
    goodFeaturesToTrack(frame1, features1, 200, 0.01  , 30 );//corner
    calcOpticalFlowPyrLK(frame1, frame2, features1, features2, status, err );

    for(size_t i=0; i < status.size(); i++)
    {
        if(status[i])
        {
            goodFeatures1.push_back(features1[i]);
            goodFeatures2.push_back(features2[i]);
        }
    }

    //All the parameters scale, angle, and translation are stored in affine
    affine = estimateRigidTransform(goodFeatures1, goodFeatures2, false);

    //std::cout<< "affine" << std::endl;
    //std::cout<<affine << std::endl;
    //flush(cout);

    //affine = affineTransform(goodFeatures1 , goodFeatures2);

    dx = affine.at<double>(0,2);
    dy = affine.at<double>(1,2);
    da = atan2(affine.at<double>(1,0), affine.at<double>(0,0));
    ds_x = affine.at<double>(0,0)/cos(da);
    ds_y = affine.at<double>(1,1)/cos(da);

    sx = ds_x;
    sy = ds_y;

    /*
    sum_transX += dx;
    sum_transY += dy;
    sum_thetha += da;
    sum_scaleX += ds_x;
    sum_scaleY += ds_y;
    */


    //reset_param(diff_scaleX, &ob_scaleX );
    //reset_param(diff_scaleY, &ob_scaleY );
    //reset_param(diff_transX, &ob_transX );
    //reset_param(diff_transY, &ob_transY );
    //reset_param(diff_thetha, &ob_thetha );

    ob_scaleX +=  ds_x-1 ;
    ob_scaleY +=  ds_y-1 ;
    ob_thetha +=  da ;
    ob_transX +=  dx ;
    ob_transY +=  dy ;



    //Don't calculate the predicted state of Kalman Filter on 1st iteration
    if(k==1)
    {
        k++;
    }
    else
    {
        Kalman_Filter(
    ob_scaleX, ob_scaleY, ob_thetha, ob_transX, ob_transY,
                &scaleX , &scaleY , &thetha , &transX , &transY);
    }

    diff_scaleX = scaleX - ob_scaleX ;
    diff_scaleY = scaleY - ob_scaleY ;
    diff_transX = transX - ob_transX ;
    diff_transY = transY - ob_transY ;
    diff_thetha = thetha - ob_thetha ;


    ds_x = ds_x + diff_scaleX;
    ds_y = ds_y + diff_scaleY;
    dx = dx + diff_transX;
    dy = dy + diff_transY;
    da = da + diff_thetha;

    //Creating the smoothed parameters matrix
    smoothedMat.at<double>(0,0) = sx * cos(da);
    smoothedMat.at<double>(0,1) = sx * -sin(da);
    smoothedMat.at<double>(1,0) = sy * sin(da);
    smoothedMat.at<double>(1,1) = sy * cos(da);

    smoothedMat.at<double>(0,2) = dx;
    smoothedMat.at<double>(1,2) = dy;

    //Uncomment if you want to see smoothed values
    //std::cout<<smoothedMat << std::endl;
    //flush(cout);

    //Warp the new frame using the smoothed parameters
    warpAffine(frame_1, smoothedFrame, smoothedMat, frame_2.size());

	//	imshow("kensyou",smoothedFrame);

    //Crop the smoothed frame a little to eliminate black region due to Kalman Filter
    smoothedFrame = smoothedFrame(cv::Range(vert_border, smoothedFrame.rows-vert_border), cv::Range(HORIZONTAL_BORDER_CROP, smoothedFrame.cols-HORIZONTAL_BORDER_CROP));

    resize(smoothedFrame, smoothedFrame, frame_2.size());

    //Change the value of test if you want to see both unstabilized and stabilized video
    if(test)
    {
        cv::Mat canvas = cv::Mat::zeros(frame_2.rows, frame_2.cols*2+10, frame_2.type());

        frame_1.copyTo(canvas(cv::Range::all(), cv::Range(0, smoothedFrame.cols)));

        smoothedFrame.copyTo(canvas(cv::Range::all(), cv::Range(smoothedFrame.cols+10, smoothedFrame.cols*2+10)));

        if(canvas.cols > 1920){
            resize(canvas, canvas, cv::Size(canvas.cols/2, canvas.rows/2));
        }
        imshow("before and after", canvas);
				imshow("before",frame_1);
				imshow("after",smoothedFrame);
    }

    return smoothedFrame;

}


//Kalman Filter implementation
void VideoStab::reset_param(double smoothedParam, double *ob_param){
    if(std::abs(smoothedParam) < 1e-2){
        *ob_param = 0.0;
    }
}
void VideoStab::Kalman_Filter(
        double ob_scaleX, double ob_scaleY,
        double ob_thetha , double ob_transX , double ob_transY,
        double *scaleX, double *scaleY,
        double *thetha , double *transX , double *transY)
{
    double frame_1_scaleX = *scaleX;
    double frame_1_scaleY = *scaleY;
    double frame_1_thetha = *thetha;
    double frame_1_transX = *transX;
    double frame_1_transY = *transY;

    double frame_1_errscaleX = errscaleX + Q_scaleX;
    double frame_1_errscaleY = errscaleY + Q_scaleY;
    double frame_1_errthetha = errthetha + Q_thetha;
    double frame_1_errtransX = errtransX + Q_transX;
    double frame_1_errtransY = errtransY + Q_transY;

    double gain_scaleX = frame_1_errscaleX / (frame_1_errscaleX + R_scaleX);
    double gain_scaleY = frame_1_errscaleY / (frame_1_errscaleY + R_scaleY);
    double gain_thetha = frame_1_errthetha / (frame_1_errthetha + R_thetha);
    double gain_transX = frame_1_errtransX / (frame_1_errtransX + R_transX);
    double gain_transY = frame_1_errtransY / (frame_1_errtransY + R_transY);

    *scaleX = frame_1_scaleX + gain_scaleX * (ob_scaleX - frame_1_scaleX);
    *scaleY = frame_1_scaleY + gain_scaleY * (ob_scaleY - frame_1_scaleY);
    *thetha = frame_1_thetha + gain_thetha * (ob_thetha - frame_1_thetha);
    *transX = frame_1_transX + gain_transX * (ob_transX - frame_1_transX);
    *transY = frame_1_transY + gain_transY * (ob_transY - frame_1_transY);

    errscaleX = ( 1 - gain_scaleX ) * frame_1_errscaleX;
    errscaleY = ( 1 - gain_scaleY ) * frame_1_errscaleX;
    errthetha = ( 1 - gain_thetha ) * frame_1_errthetha;
    errtransX = ( 1 - gain_transX ) * frame_1_errtransX;
    errtransY = ( 1 - gain_transY ) * frame_1_errtransY;
}
