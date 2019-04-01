#include "imageprocess.h"

using namespace cv ;
using namespace std ;
void resize_size(Mat&src, Mat&out, int width, int Height)
{
    resize(src, out, cv::Size(width, Height));
}

void resize_factor(Mat& src, Mat& out, double fx, double fy)
{

    resize(src, out, cv::Size(0, 0));
}

void gray_image(Mat&src, Mat&out)
{
    cv::cvtColor(src, out, CV_BGR2GRAY);
}

QImage Mat2QImage(const Mat& src)
{
    if(!src.data)
    {
        throw "image data is none ";
    }
    if(src.type() == CV_8UC1)
    {
         QImage image(src.cols, src.rows, QImage::Format_Indexed8);
         image.setColorCount(256);
         for(int i = 0; i < 256; i++)
            {
                image.setColor(i, qRgb(i, i, i));
            }
         uchar *pSrc = src.data;
         for(int row = 0; row < src.rows; row ++)
         { uchar *pDest = image.scanLine(row);
             memcpy(pDest, pSrc, src.cols);
             pSrc += src.step;
         }
         return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(src.type() == CV_8UC3)
    { // Copy input Mat
        const uchar *pSrc = (const uchar*)src.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, src.cols, src.rows, src.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(src.type() == CV_8UC4) {

        // Copy input Mat
        const uchar *pSrc = (const uchar*)src.data;
        // Create QImage with same dimensions as input Mat
         QImage image(pSrc, src.cols, src.rows, src.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else {
        return QImage();
    }

}

uchar getGrayVal(Mat& img, int row, int col, int channel=0)
{
    try
    {
        if(row < 0|| (row > (img.rows-1)) || col < 0 || (col > (img.cols-1)))
        {

            throw "Invalid Index";
        }
        int channelNum = img.channels();
        switch (channelNum) {
        case 1:
            if(channel != 0)
            {
                throw "灰度图，为单通道";
            }
            return img.at<uchar>(row, col);
            break;
        case 3:
            return img.at<Vec3b>(row, col)[channel];
            break;
        default:
            return 0;
            break;
        }
    }catch(Exception e)
    {
        return 0;
    }
}

bool setGrayVal(Mat& img, int row, int col, int channel, uchar grayVal)
{
    try
    {
        if(row < 0|| (row > (img.rows-1)) || col < 0 || (col > (img.cols-1)))
        {

            throw "Invalid Index";

        }
        int channelNum = img.channels();
        switch (channelNum) {
        case 1:
            if(channel != 0)
            {
                throw "灰度图，为单通道";
            }
            img.at<Vec3b>(row, col)[0] = grayVal;
            break;
        case 3:
            img.at<Vec3b>(row, col)[channel] = grayVal;
            break;
        default:
            return 0;
            break;
        }
    }catch(Exception e)
    {
        return 0;
    }
}

bool dftImg(const cv::Mat &inputImage, cv::Mat &outputImage)       //输出的为float类型图像，无法转成QImage,需要用Imageshow显示
{

    Mat grayImg;

    if(inputImage.channels() == 3)
    {
        cvtColor(inputImage, grayImg, CV_BGR2GRAY);
    }
    else
    {
        grayImg = inputImage.clone();
    }

    int optH = getOptimalDFTSize( grayImg.rows );
    int optW = getOptimalDFTSize( grayImg.cols );

    Mat padded;
    copyMakeBorder(grayImg,
                   padded,
                   0,
                   optH - grayImg.rows,
                   0,
                   optW - grayImg.cols,
                   BORDER_CONSTANT,
                   Scalar::all(0));

    Mat channels[] = {Mat_<float>(padded),
                    Mat::zeros(padded.size(),
                    CV_32F)};
    Mat complex;
    merge(channels, 2, complex);

    dft(complex, complex);

    split(complex, channels);
    Mat mag;
    magnitude(channels[0], channels[1], mag);

    mag += Scalar::all(1);
    log(mag, mag);

    mag = mag(Rect(0,
                   0,
                   mag.cols & -2,
                   mag.rows & -2));

    int cx = mag.cols/2;
    int cy = mag.rows/2;

    Mat q0(mag, Rect(0, 0, cx, cy));
    Mat q1(mag, Rect(cx, 0, cx, cy));
    Mat q2(mag, Rect(0, cy, cx, cy));
    Mat q3(mag, Rect(cx, cy, cx, cy));

    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    normalize(mag, mag, 0, 255, CV_MINMAX);

    Mat_<uchar> magI8bit(mag);
    outputImage = magI8bit.clone();

}

void reduce_domain(const cv::Mat& srcImg, Rect ROI, Mat& outImg )
{

    outImg = srcImg(ROI); //这样写原图也会变

}




void crop_domain(Mat& srcImg, Rect ROI, Mat& outImg )
{
     outImg = srcImg(ROI).clone();
}

void convert_image(Mat& srcImg, Mat& outImg)
{
   bitwise_not(srcImg, outImg);
}

void gen_gauss_pyramid(Mat& srcImg, std::vector<Mat>outImgs, int maxLevel)
{
    buildPyramid(srcImg, outImgs, maxLevel);
}
/**
* brief         由对应的三个点的点集得到仿射矩阵
* param        src和dst为一个包含3个点集的数组头，
* date         2019-03-27
* return
*/
void getTransFormMat(const Point2f*src, const Point2f*dst, Mat& outMat)
{
    outMat = getAffineTransform(src, dst);
}
/**
* brief         利用旋转中心点，角度和比例计算仿射矩阵
* param        Waleon
* date         2019-03-27
* return
*/
void gerRotationMat(const Point2f&center, double angle, double scale,  Mat& outMat)
{
    outMat = getRotationMatrix2D(center, angle, scale);
}

/**
* brief          仿射变换
* param        HotMat2D为2x3的仿射矩阵
* date         2019-03-27
* return
*/
void affine_trans_image(Mat& srcImg, Mat& outImg, const Mat& HotMat2D)
{
    warpAffine(srcImg, outImg, HotMat2D, srcImg.size());
}

void reduce_domain(Mat& srcImg, Mat& outImg, vector<Point> coutour)
{
    Mat mask = Mat(srcImg.size(), srcImg.type(), Scalar::all(0));
    cv::fillPoly(mask, coutour, Scalar::all(255));
    srcImg.copyTo(outImg, mask);

}


void gen_gaussFilter(int width, int height, double sigma1, double sigma2,
                     Mat& filter, bool is_reverse)
{
    Mat kernelX = getGaussianKernel(width, sigma1, CV_32F);  //kernel size,sigma
    Mat kernelY = getGaussianKernel(height, sigma2, CV_32F);  //kernel size,sigma
    Mat tempfilter;
    tempfilter =  kernelX * kernelY.t();
    if(is_reverse)
    {

        Mat tmpMat(tempfilter.rows,tempfilter.cols, tempfilter.type(), Scalar::all(0));
        tempfilter = tmpMat - tempfilter;
        cv::normalize(tempfilter, tempfilter, 1,0,cv::NORM_MINMAX);
        Mat channels[] = {tempfilter, tempfilter.clone()};
        merge(channels, 2, filter);
    }
    else
    {
        cv::normalize(tempfilter, tempfilter, 1,0,cv::NORM_MINMAX);
        Mat channels[] = {tempfilter, tempfilter.clone()};
        merge(channels, 2, filter);
    }

}


void freqfilt(Mat& input,Mat& blur, Mat& idftImage, Mat& dftImage, int x, int y)
{
    Mat grayImage;
    if(input.channels() == 3)
    {
        cvtColor(input, grayImage, cv::COLOR_BGR2GRAY);
    }
    else
    {
        grayImage = input.clone();
    }

    int w=getOptimalDFTSize(input.cols);  //获取进行dtf的最优尺寸
    int h=getOptimalDFTSize(input.rows); //获取进行dtf的最优尺寸
    Mat padded;
    copyMakeBorder(input,padded, 0, h-input.rows, 0, w-input.cols,
                   BORDER_CONSTANT , Scalar::all(0));  //边界填充
    padded.convertTo(padded,CV_32FC1); //将图像转换为flaot型
    //***********************DFT*******************
    Mat plane[]={padded, Mat::zeros(padded.size() , CV_32FC1)}; //创建通道，存储dft后的实部与虚部（CV_32F，必须为单通道数）
    Mat complexIm;
    merge(plane,2,complexIm);//合并通道 （把两个矩阵合并为一个2通道的Mat类容器）
    dft(complexIm,complexIm);//进行傅立叶变换，结果保存在自身
    //***************中心化********************
    split(complexIm,plane);//分离通道（数组分离）
    Centralization(plane[0]);
    Centralization(plane[1]);
    //*****************滤波器函数与DFT结果的乘积****************
    Mat filter_padded;
    if(y >=0)
    {
       //复制边缘像素
       copyMakeBorder(blur, filter_padded, 0, y, x, 0, BORDER_REPLICATE);
       blur = Mat(filter_padded, Rect(0,y,blur.cols, blur.rows));
    }
    else
    {
        copyMakeBorder(blur, filter_padded, abs(y), 0, x, 0, BORDER_REPLICATE);
        blur = Mat(filter_padded, Rect(0,0,blur.cols, blur.rows));
    }
    merge(plane,2,complexIm);
    Mat ROI(complexIm,Rect(0, 0,blur.cols, blur.rows));
    multiply(ROI, blur, ROI);
    //滤波后分离通道显示滤波后的dft图像
    //象限对称，以右半边象限为准心
    split(complexIm,plane);
    replicate_coord(plane[0]);
    replicate_coord(plane[1]);
    magnitude(plane[0],plane[1],plane[0]);
    plane[0]+=Scalar::all(1);
    log(plane[0],plane[0]);
    normalize(plane[0],plane[0],1,0,CV_MINMAX);
    dftImage = plane[0].clone();
    idft(complexIm, complexIm);	//idft结果也为复数
    split(complexIm,plane);//分离通道，主要获取通道
    magnitude(plane[0],plane[1],plane[0]);  //求幅值(模)
    normalize(plane[0],plane[0],1,0,CV_MINMAX);  //归一化便于显示
    idftImage = plane[0].clone();
}

void Centralization(Mat& input)
{

    int cx=input.cols/2;int cy=input.rows/2;//以下的操作是移动图像  (零频移到中心)
    Mat part1_r(input, Rect(0,0,cx,cy));  //元素坐标表示为(cx,cy)
    Mat part2_r(input, Rect(cx,0,cx,cy));
    Mat part3_r(input, Rect(0,cy,cx,cy));
    Mat part4_r(input, Rect(cx,cy,cx,cy));
    Mat temp;
    part1_r.copyTo(temp);  //左上与右下交换位置(实部)
    part4_r.copyTo(part1_r);
    temp.copyTo(part4_r);
    part2_r.copyTo(temp);  //右上与左下交换位置(实部)
    part3_r.copyTo(part2_r);
    temp.copyTo(part3_r);
}
void replicate_coord(Mat& input)
{
    int cx=input.cols/2; int cy=input.rows/2;//以下的操作是移动图像  (零频移到中心)
    Mat image_fliped;
    flip(input, image_fliped, -1);
    Mat input_tl(input, Rect(0,0,cx,cy));  //元素坐标表示为(cx,cy)
    Mat fliped_tl(image_fliped, Rect(0,0,cx,cy));
    Mat input_ll(input, Rect(0,cy,cx,cy));
    Mat flip_ll(image_fliped, Rect(0,cy,cx,cy));
    fliped_tl.copyTo(input_tl);
    flip_ll.copyTo(input_ll);

}

