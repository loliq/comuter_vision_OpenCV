#include <iostream>
#include "cvregion.h"  //头文件和实现文件分离即可不报错
using namespace std;
//using namespace cv;
void processImage(const cv::Mat &inputImage, cv::Mat &outputImage, Mat mask)
{
    using namespace cv;
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
    normalize(mag, mag, 0, 255, CV_MINMAX); //0-255归一化
    Mat_<uchar> magI8bit(mag);
    outputImage = magI8bit.clone();

}

int main(int argc, char *argv[])
{

    Mat srcImg = imread("/home/lilanluo/image.bmp",-1); //xuyao fangzai debugdewenjianjia
    Mat outImg;
    cv_threshold(srcImg,outImg, 150, 255);
    std::vector<Region> regions;
    connection(outImg, regions);

//    Mat kernelX = getGaussianKernel(3, 1);  //kernel size,sigma
//    Mat kernelY = getGaussianKernel(3, 4);  //kernel size,sigma
//    Mat G = kernelX * kernelY.t(); //二维高斯核
////    processImage(srcImg, outImg);
//    cout << G <<endl << endl <<endl;
    imshow("original_mat", outImg);
    waitKey(-1);
//    cout << G << endl << endl << endl;
    //高斯核




    return 0;
}
