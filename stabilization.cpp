#include "stabilization.h"

void Stabilization::getSmoothedFrame(cv::Mat &frame, cv::Mat &dst_img)
{
    try{
        if(frame.data == NULL)
        {
            std::cerr<<"Stalilization frame data is nothing"<<std::endl;
            exit(0);
        }
        else if( (pre_frame.data == NULL)){
            pre_frame = frame.clone(); //frame_1
            convertGray(pre_frame,frame1);//frame1
        }
        else{
            convertGray(frame, frame2);//frame2
            dst_img = stabilize(pre_frame, frame);
            pre_frame = frame.clone();//frame_1
            frame2.copyTo(frame1);//frame1
        }
    }catch(cv::Exception& e){
            std::cerr<<"Exception Stalilization::getSmoothedFrame()"<<std::endl;
            pre_frame = frame.clone();//frame_1
            convertGray(pre_frame,frame1);//frame1
            frame.copyTo(dst_img);//dst_img
    }
}

void Stabilization::getSmoothedFrameAndMat( cv::Mat &frame, cv::Mat &dst_img, cv::Mat &dst_Mat){
    getSmoothedFrame(frame,dst_img);
    dst_Mat = smoothedMat.clone();
}
void Stabilization::convertGray( cv::Mat &src , cv::Mat &dst_img ){
    if((src.channels()==1)){
        dst_img = src.clone();
    }
    else{
        cv::cvtColor(src, dst_img, cv::COLOR_BGR2GRAY);
    }
}
