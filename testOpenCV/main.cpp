#include <iostream>
#include <time.h>
#include "ImageProcess/cvregion.h"  //头文件和实现文件分离即可不报错
#include "ImageProcess/imageprocess.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;


int main(int argc, char *argv[])
{
    Mat input = imread("/home/lilanluo/image.bmp", 0); //xuyao fangzai debugdewenjianjia
    Mat outImg;
    Mat gaussianKernel;
    gen_gaussFilter(input.rows, input.cols, 301, 201, gaussianKernel);
    Mat fftImg, flitedImg;
    freqfilt(input, gaussianKernel, flitedImg, fftImg, 150, 200);


    imshow("结果图 sigma=40",fftImg);
    waitKey(-1);
    return 0;
}
