#include<opencv2/opencv.hpp>
#include<cmath>
//do dilate
cv::Mat my_dilate(const cv::Mat& src)
 {
    cv::Mat dst;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(3,3));
    cv::dilate(src,dst,kernel,cv::Point(-1,-1),1);
    return dst;
 }

 //conbine two desperate lights
 std::vector<cv::Point> get_all_point(const cv::Mat& src)
 {
    std::vector<std::vector<cv::Point>> all;
    std::vector<cv::Point> ans;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(src , all , hierarchy , cv::RETR_TREE ,cv::CHAIN_APPROX_SIMPLE);
    for(int i  = 0 ; i < all.size() ; i ++)
    {
        if(hierarchy[i][2] == -1 && contourArea(all[i]) >= 10)
        {
           for(int j = 0 ; j < all[i].size() ; j++)
                ans.push_back(all[i][j]);
        }
    }
    return ans;
 }

 //draw the arrow of axis
void drawArrow (const cv::Mat& img,const cv::Point2f start,const cv::Point2f end,const cv::Scalar color) {
        // 简单箭头头部
        cv::Point2f dir = end - start;
        float length = cv::norm(dir);
        if (length > 0) {
            dir = dir * (1.0 / length); //standard vector
            cv::Point2f perpendicular(-dir.y, dir.x); //get vertical vector
            cv::Point2f arrow1 = end - dir * 8 + perpendicular * 4;
            cv::Point2f arrow2 = end - dir * 8 - perpendicular * 4;
            cv::line(img, end, arrow1, color, 2 , cv::LINE_AA);
            cv::line(img, end, arrow2, color, 2 , cv::LINE_AA);
        }
}

//draw the axis
void drawaxis(const cv::Mat& src , const cv::RotatedRect& rrect , const cv::Mat& rVec , const cv::Mat& tVec , const cv::Mat cam , const cv::Mat dis)
 {
    float axis_length = 100.0f;
    std::vector<cv::Point3f> axis_points = {
        {0, 0, 0},           // origin
        {axis_length, 0, 0}, // X
        {0, axis_length, 0}, // Y
        {0, 0, axis_length}  // Z
    };
    
    // 投影坐标轴
    std::vector<cv::Point2f> projected_axis;
    //calculate the points of axis based on the pnp
    cv::projectPoints(axis_points, rVec, tVec, cam, dis, projected_axis);
    
    //the point of axis on the picture
    cv::Point2f origin = projected_axis[0];
    cv::Point2f x_end = projected_axis[1];
    cv::Point2f y_end = projected_axis[2];
    cv::Point2f z_end = projected_axis[3];
    std::vector<cv::Scalar> colo = {cv::Scalar(0, 0, 255),cv::Scalar(0, 255, 0),cv::Scalar(255, 0, 0)};
    
      //std::cout<< projected_axis<<std::endl;
    for(int i = 1 ; i <= 3 ;i++)
    {
      if(cv::norm(projected_axis[i] - origin) > 0)
      {
         cv::line(src, origin, projected_axis[i], colo[i-1], 2 , cv::LINE_AA);
         drawArrow(src, origin, projected_axis[i], colo[i-1]);
         //std::cout<<colo[i]<<" ";
      } 
    }
}

//change the points of rotatedrect to fit the origin order of board
//p[0] is the tl
std::vector<cv::Point2f> rear_point(const cv::RotatedRect& rrect)
{
   std::vector<cv::Point2f> rrect_point;
   cv::Point2f vec[4];
   rrect.points(vec);
   for(int i = 0 ; i<= 3 ; i++)
   {
      //std::cout<<vec[i]<<" ";
   }
   if(rrect.size.width/rrect.size.height > 1)
   {
      cv::Point2f t = vec[0] ; vec[0] = vec[1];
         vec[1] = vec[2];
         vec[2]  = vec[3];
         vec[3] = t;
   }
   rrect_point.assign(vec , vec+4);
   //std::cout<<rrect_point<<std::endl;
   return rrect_point;
}

void give_pnp(const cv::RotatedRect& rrect , const cv::Mat& src)
 {
   double hwidth = 130.0 / 2.0, hheight = 50 / 2.0; 
   //board points
   std::vector<cv::Point3f> obj = std::vector<cv::Point3f> {
         cv::Point3f(0, -hwidth , hheight ),
         cv::Point3f(0,hwidth , hheight ),
         cv::Point3f(0,hwidth , -hheight ),
         cv::Point3f(0,-hwidth , -hheight )
      };
      //std::cout<<rrect.size.width/rrect.size.height<<" ";
      std::vector<cv::Point2f> rrect_points = rear_point(rrect);
      cv::Mat rVec = cv::Mat::zeros(3 , 1, CV_64FC1);
      cv::Mat tVec = cv::Mat::zeros(3 , 1, CV_64FC1);
      double ca[3][3] = {{9.28130989e+02 , 0 , 3.77572945e+02 }, {0 , 9.30138391e+02 ,2.83892859e+02 },{0, 0 , 1.0000}};
      cv::Mat cam(3 , 3, CV_64FC1 , ca);
      double di[1][5] = {{-2.54433647e-01 , 5.69431382e-01, 3.65405229e-03, -1.09433818e-03, -1.33846840e+00}};
      cv::Mat dis(1 , 5 , CV_64FC1 , di);
      solvePnPRansac(obj , rrect_points , cam , dis , rVec , tVec);
      //std::cout<<tVec.t()<<std::endl; 
      drawaxis(src , rrect , rVec , tVec , cam , dis);
 }

