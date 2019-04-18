#ifndef CVREGION_H
#define CVREGION_H
#include <opencv2/opencv.hpp>
#include <algorithm>
using namespace cv;
using namespace std;

/**
 * @brief The run_code struct
 * 行程编码结构定义
 */
struct run_code
{
    int row;
    int beginColum;
    int endColum;
};
enum RegionFeature
{
    Area,
    Width,
    Height,
    Row,
    Column,
};

/**
 * @brief The AreaCmp_int struct - 比较vector中两个int型数据的大小
 */
struct Cmp_Int
{
    Cmp_Int(const vector<int>& _vecOfInt): vecOfInt(& _vecOfInt){} //初始化列表进行初始化
    bool operator ()(int a, int b) const{return (*vecOfInt)[a] > (*vecOfInt)[a];}
    const vector<int> *vecOfInt;
};

/**
 * @brief The Region class
 * 统一定义二值区域中元素为0和255
 */
class Region      //存储的时候想办法用稀疏矩阵存储，要使用的时候恢复成正常的稠密矩阵
{
public:
    Region();
    ~Region();
    Region(Mat& mat, const cv::Rect& box, const Point2d& center_point, const int& Area);
    //调用一个const对象的非const成员函数这是不允许的
    int getArea() const;
    int getWidth() const;
    int getHeight() const;
    int getBeginRow() const;
    int getBeginColumn() const;
    Point2d getCenter_point() const;
    Rect getBounding_box() const;
    void move_reigon(int row_offset, int colunOffset);
private:
    vector<run_code> region_codes;  //行程编码
    cv::Size Mat_size;
    cv::Rect bounding_box;  //包括了轮廓的宽高信息
    Point2d center_point;
    int area;
};
/**
 * @brief cv_threshold -阈值分割，二值化阈值
 * @param srcImg
 * @param outImg
 * @param lowVal
 * @param hightVal
 */
extern void cv_threshold(Mat& srcImg, Mat& outImg, double lowVal, double hightVal);
/**
 * @brief connection  将二值化后的图像分割成连通域
 * @param binaryImg   必须是二值化图像
 * @param regions    返回的vector组织的regions
 */
extern void connection(Mat& binaryImg, std::vector<Region>& regions);
/**
 * @brief genRunCode 输入二值化图像
 * @param mat
 * @param run_codes
 */
extern void genRunCode(const Mat& input_mat, vector<run_code>& run_codes);
/**
 * @brief genRunCode -输入最小外接矩形box，生成行程编码
 * @param mat -必须是二值化图像
 * @param box
 * @param run_codes
 */
extern void genRunCode(const Mat& input_mat, const Rect& bounding_box, vector<run_code>& run_codes);
/**
 * @brief sort_regions_index 排序region,输出排序索引列表
 * @param regions
 * @param feature -排序参考的特征
 * @param sortIdx
 * @param is_reverse 是否反向,默认从小到大排列
 */
//既可以在类的声明中，也可以在函数定义中声明缺省参数，但不能既在类声明中又在函数定义中同时声明缺省参数。
extern void sort_regions_index(vector<Region>& regions,RegionFeature feature,
                        vector<int>& sortIdx, bool is_reverse = false);
/**
 * @brief sort_regions 排序region,输出排序后的region
 * @param regions
 * @param feature  -排序参考的特征
 * @param sortedRegions
 * @param is_reverse  是否反向 默认从小到大排列
 */
extern void sort_regions(vector<Region> &regions,RegionFeature feature,
                  vector<Region>& sortedRegions, bool is_reverse = false);
/**
 * @brief select_region_shape
 * @param regions
 * @param feature
 * @param select_regions
 * @param minVal
 * @param maxVal
 */
extern void select_region_shape(vector<Region> &regions, RegionFeature feature,
                         vector<Region>& select_regions, float minVal, float maxVal);
#endif // CVREGION_H
