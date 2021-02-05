#ifndef STABILIZATION
#define STABILIZATION

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/flann/flann.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <time.h>

#include "opencv4/opencv2/xfeatures2d/nonfree.hpp"
#include "my_videostab.h"


class Stabilization:public VideoStab
{
    public:
        Stabilization():VideoStab(){};
        ~Stabilization(){};
        void getSmoothedFrame( cv::Mat &frame , cv::Mat &dst_img);
        void getSmoothedFrameAndMat( cv::Mat &frame , cv::Mat &dst_img, cv::Mat &dst_Mat);
    protected:
        void convertGray( cv::Mat &src , cv::Mat &dst_img );
    private:
        const int HORIZONTAL_BORDER_CROP = 30;
        bool count_flag = false;
        //VideoStab stab;
        cv::Mat pre_frame; //frame_1
        cv::Mat gray_frame;
        cv::Mat smoothedFrame;
};
#endif //STABILIZATION
