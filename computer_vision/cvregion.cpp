#include "cvregion.h"
Region::Region()
{

}
Region::~Region()
{

}
Region::Region(Mat& mat, const cv::Rect& box, const Point2d& center_point, const int& Area)
{

    this->mat_region = mat.clone();
    this->bounding_box = box;
    this->center_point = center_point;
    this->area = Area;
}



/**
* brief         二值化灰度图像,(lowval, hightVal)中的灰度值为1，其余为0
* param        Waleon
* date         2019-03-27
* return
*/
void cv_threshold(Mat& srcImg, Mat& outImg, double lowVal, double hightVal)
{
    if(srcImg.channels()!=1)
    {
        throw "阈值化操作需要为灰度图";
    }
    Mat lowImg;
    threshold(srcImg, lowImg, lowVal, 1, cv::THRESH_BINARY);
    Mat hightImg;
    threshold(srcImg, hightImg, hightVal, 1, cv::THRESH_BINARY_INV);
    bitwise_and(lowImg, hightImg, outImg);
}

/**
* brief         输入二值图,返回连通区域集
* param        Waleon
* date         2019-03-27
* return
*/
void connection(Mat& binaryImg, std::vector<Region> regions)
{
    Mat labels, stats, centroids;
    int nccomps = connectedComponentsWithStats(binaryImg, labels, stats, centroids);
    std::vector<Mat> region_mats;
    //标签输出的结果包括了背景标签，背景标签的索引为0
    for(int i=0; i < nccomps; ++i)
    {
        Mat tmp_img = Mat::zeros(binaryImg.size(), CV_8U);
        region_mats.push_back(tmp_img);
    }
    //label 0 为背景标签
    for(int y = 0 ; y< binaryImg.rows; ++y)
    {
        for(int x = 0; x < binaryImg.cols; ++x)
        {
            int label = labels.at<int>(y, x);
            CV_Assert(0 <= label && label <= nccomps);
            if(label) //label为0的时候为背景
            {
                region_mats[label-1].at<uchar>(y, x) = 1; //将区域的灰度设置为1
            }

        }
    }
    for(int i = 1; i <= nccomps; ++i) //
    {
        int width = stats.at<int>(i, cv::CC_STAT_WIDTH);
        int height = stats.at<int>(i, cv::CC_STAT_HEIGHT);
        int cord_x =  stats.at<int>(i, cv::CC_STAT_LEFT);
        int cord_y =  stats.at<int>(i, cv::CC_STAT_TOP);
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        Rect box = Rect(cord_x, cord_y, width, height);
        Point2f center_point = Point2d(centroids.at<int>(i, 0), centroids.at<int>(i, 1));
        Region tm_region(region_mats[i-1], box, center_point, area);
        regions.push_back(tm_region);
    }


}
