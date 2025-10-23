#include "need.h"
#include<iostream>
#include<opencv2/opencv.hpp>
#include<filesystem>

int main()
{
    cv::VideoCapture cap("../vedio/vedio.avi");
    cv::VideoWriter wrt;
    wrt.open("../vedio/vedio_out.avi" , cv::VideoWriter::fourcc('I','4','2','0') , cap.get(cv::CAP_PROP_FPS)/3.5 ,
             cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH) , cap.get(cv::CAP_PROP_FRAME_HEIGHT)) , true);
             std::cout<<cap.get(cv::CAP_PROP_FPS)<<std::endl;
    if (!wrt.isOpened()) {
    std::cerr << "错误：无法创建VideoWriter！" << std::endl;
    return -1;
    }
    
    if (!cap.isOpened()) {
        std::cout << "Error: Could not open video file" << std::endl;
        return -1;
    }
    cv::Mat vedio_chip;
    cv::Mat ori;
    while(true)
    {
        cv::Point2f rect[4];
        cap >> vedio_chip;
        cap >> ori;
        if(vedio_chip.empty()) break;
        cv::Mat black;
        cv::cvtColor(vedio_chip , black , cv::COLOR_BGR2GRAY);
        cv::threshold(black , black , 100 , 255 , cv::THRESH_BINARY);
        black = my_dilate(black);
        // cv::imshow("black",black);
        // cv::waitKey(100);
        std::vector<cv::Point> all_point = get_all_point(black);
        //to avoid the lost of target
        if(!all_point.empty())
        {
            cv::RotatedRect mar = minAreaRect(all_point);
            mar.points(rect);
            for(int i = 0 ; i < 4 ; i++)
            {
                 cv::line(vedio_chip , rect[i] , rect[(i+1)%4] , cv::Scalar(0,255,0),2 , cv::LINE_AA);
            }
            
            give_pnp(mar , vedio_chip);
            wrt<<vedio_chip;
            cv::waitKey(1000/cap.get(cv::CAP_PROP_FPS));
            cv::imshow("output",vedio_chip);
            cv::imshow("origin" ,ori);
            cv::waitKey(200);
        }
    }
    cap.release();
    wrt.release();
}