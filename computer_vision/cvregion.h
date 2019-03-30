#ifndef CVREGION_H
#define CVREGION_H
#include <opencv2/opencv.hpp>
using namespace cv;

/**
* @className     Region
* @brief         摘要
* @date          2019-03-27
*/
class Region      //存储的时候想办法用稀疏矩阵存储，要使用的时候恢复成正常的稠密矩阵
{
public:
    Region();
    ~Region();
    Region(Mat& mat, const cv::Rect& box, const Point2d& center_point, const int& Area);
private:
    Mat mat_region; //必须为二值图像
    cv::Rect bounding_box;  //包括了轮廓的宽高信息
    Point2d center_point;
    int area;
    std::vector<std::vector<cv::Point>> coutours;  //轮廓
};
extern void cv_threshold(Mat& srcImg, Mat& outImg, double lowVal, double hightVal);
extern void connection(Mat& binaryImg, std::vector<Region> regions);
#endif // CVREGION_H
