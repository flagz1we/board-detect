#include<opencv2/opencv.hpp>

cv::Mat my_dilate(const cv::Mat& src);

std::vector<cv::Point> get_all_point(const cv::Mat& src);

void give_pnp(const cv::RotatedRect& rrect , const cv::Mat& src);

void drawArrow (cv::Mat& img, cv::Point2f start, cv::Point2f end, cv::Scalar color);

void drawaxis(const cv::Mat& src , const cv::RotatedRect& rrect , const cv::Mat& rVec , const cv::Mat& tVec , const cv::Mat cam , const cv::Mat dis);
