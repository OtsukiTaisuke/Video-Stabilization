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
#include "my_videostab.h"

#include "opencv4/opencv2/xfeatures2d/nonfree.hpp"
//#include "opencv2/nonfree/nonfree.hpp"



//using namespace std;
//using namespace cv;

// This class redirects cv::Exception to our process so that we can catch it and handle it accordingly.
class cvErrorRedirector {
public:
    int cvCustomErrorCallback( )
    {
        std::cout << "A cv::Exception has been caught. Skipping this frame..." << std::endl;
        return 0;
    }

    cvErrorRedirector() {
        cvRedirectError((cv::ErrorCallback)cvCustomErrorCallback(), this);
    }
};

const int HORIZONTAL_BORDER_CROP = 30;

clock_t start;
clock_t end;
double ti;
double ave_ti;
int main(int argc, char **argv)
{
    cvErrorRedirector redir;
    //Create a object of stabilization class
    VideoStab stab;

    //Initialize the VideoCapture object
    cv::VideoCapture cap(0);

    cv::Mat frame_2, frame2;
    cv::Mat frame_1, frame1;

    cap >> frame_1;
    cv::cvtColor(frame_1, frame1, cv::COLOR_BGR2GRAY);

    cv::Mat smoothedMat(2,3,CV_64F);

    cv::VideoWriter outputVideo;
    outputVideo.open("com.avi" , CV_FOURCC('X' , 'V' , 'I' , 'D'), 30 , frame_1.size());

    while(true)
    {
			start = clock();
        try {
            cap >> frame_2;

            if(frame_2.data == NULL)
            {
                break;
            }

            cv::cvtColor(frame_2, frame2, cv::COLOR_BGR2GRAY);

            cv::Mat smoothedFrame;

            smoothedFrame = stab.stabilize(frame_1 , frame_2);

            outputVideo.write(smoothedFrame);

            imshow("Stabilized Video" , smoothedFrame);

            cv::waitKey(10);

            frame_1 = frame_2.clone();
            frame2.copyTo(frame1);
        } catch (cv::Exception& e) {
            cap >> frame_1;
            cv::cvtColor(frame_1, frame1, cv::COLOR_BGR2GRAY);
        }
			end = clock();
			ti = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;
			ave_ti = 0.5*(ave_ti + ti);
			//printf("ave_time %lf[ms]\n", ave_ti);
    }

    return 0;
}


