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
    Mat input = imread("/home/lilanluo/odd.jpg", 0); //xuyao fangzai debugdewenjianjia
    cout << "original image size " << input.size <<endl;
    int optWidth = getOptimalDFTSize(input.cols);
    int optHeight = getOptimalDFTSize(input.rows);
    Mat output;
    dftImg(input, output);


    imshow("FFt", output);
    waitKey(-1);
    cout << "FFT optimalDFT width" << optWidth << endl;
    cout << "FFT optimalDFT Height" << optHeight <<endl;

    return 0;

}
