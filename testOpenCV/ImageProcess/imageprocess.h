#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <opencv2/opencv.hpp>
#include <QImage>


/**
 * @brief resize_size
 * @param src
 * @param out
 * @param width
 * @param Height
 */
extern void resize_size(cv::Mat& src, cv::Mat& out, int width, int Height);

/**
 * @brief resize_factor
 * @param src
 * @param out
 * @param fx
 * @param fy
 */
extern void resize_factor(cv::Mat& src, cv::Mat& out, double fx, double fy);
/**
 * @brief gray_image
 * @param src
 * @param out
 */
extern void gray_image(cv::Mat&src, cv::Mat&out);
/**
 * @brief Mat2QImage
 * @param src
 * @return
 */
extern QImage Mat2QImage(const cv::Mat& src);

/**
 * @brief getGrayVal
 * @param img
 * @param row
 * @param col
 * @param channel
 * @return
 */
extern uchar getGrayVal(const cv::Mat& img, int row, int col, int channMatel=0);
/**
 * @brief setGrayVal
 * @param img
 * @param row
 * @param col
 * @param channel
 * @param grayVal
 * @return
 */
extern bool setGrayVal(cv::Mat& img, int row, int col, int channel, uchar grayVal);
/**
 * @brief dftImg
 * @param inputImage
 * @param outputImage
 * @return
 */
extern bool dftImg(const cv::Mat &inputImage, cv::Mat &outputImage);
extern void reduce_domain(const cv::Mat& srcImg, const cv::Rect ROI, cv::Mat& outImg );
extern void crop_domain(const cv::Mat& srcImg, const cv::Rect ROI, cv::Mat& outImg );

extern void convert_image(cv::Mat& srcImg, cv::Mat& outImg);

extern void gen_gauss_pyramid(cv::Mat& srcImg, std::vector<cv::Mat>outImgs, int maxLevel);
/**
* brief         由对应的三个点的点集得到仿射矩阵
* param        src和dst为一个包含3个点集的数组头，
* date         2019-03-27
* return
*/
extern void getTransFormMat(const cv::Point2f*src, const cv::Point2f*dst, cv::Mat& outMat);
/**
* brief         利用旋转中心点，角度和比例计算仿射矩阵
* param        Waleon
* date         2019-03-27
* return
*/
extern void gerRotationMat(const cv::Point2f&center, double angle, double scale, cv::Mat& outMat);

/**
* brief          仿射变换
* param        HotMat2D为2x3的仿射矩阵
* date         2019-03-27
* return
*/
extern void affine_trans_image(cv::Mat& srcImg, cv::Mat& outImg, const cv::Mat& HotMat2D);
/**
 * @brief gen_gaussFilter-生成高斯频域滤波器
 * @param width -input.rows
 * @param height -input.cols
 * @param sigma1
 * @param sigma2
 * @param filter
 * @param is_reverse -选择是陷波还是滤波
 */
extern void gen_gaussFilter(int width, int height, double sigma1, double sigma2,
                     cv::Mat& filter, bool is_reverse=true);

/**
 * @brief freqfilt-频率域滤波
 * @param input
 * @param blur   -滤波器图像，需要和input同样大小
 * @param idftImage -滤波器反变换结果
 * @param dftImage -频域图像
 * @param x -滤波器中心所在的x坐标      |x| < inout.cols/2
 * @param y -滤波器中心所在的位置-y坐标，|y| < input.rows/2
 */
extern void freqfilt(cv::Mat& input, cv::Mat& blur, cv::Mat& idftImage,
                     cv::Mat& dftImage, int x=0, int y=0);
/**
 * @brief Centralization -图像中心化
 * @param input
 */
extern void Centralization(cv::Mat& input);
/**
 * @brief replicate_coord -对图像进行关于原点对称
 * @param input
 */
void replicate_coord(cv:: Mat& input);
#endif // IMAGEPROCESS_H
