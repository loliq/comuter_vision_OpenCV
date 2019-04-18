#include <iostream>
#include <time.h>
#include <QTime>
#include "ImageProcess/cvregion.h"  //头文件和实现文件分离即可不报错
#include "ImageProcess/imageprocess.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include<vector>
using namespace std;
using namespace cv;


int main(int argc, char *argv[])
{
    Mat input = imread("/home/lilanluo/odd.jpg", 0); //xuyao fangzai debugdewenjianjia
    cout << "original image size " << input.size <<endl;
    QTime time;
    Mat binaryImg;
    cv_threshold(input, binaryImg, 150, 255);//    connection(binaryImg, connected_regions);
    vector<Region> connectied_region;
    time.start();
    //用移动拷贝60ms
    connection(binaryImg, connectied_region);
    //不用移动拷贝
    cout << "time = " << time.elapsed() << " ms" << endl;
    for(Region& r: connectied_region )
    {
        cout << r.getArea() << endl;
    }

    imshow("FFt", binaryImg);
    waitKey(-1);
//    cout << "FFT optimalDFT width" << optWidth << endl;
//    cout << "FFT optimalDFT Height" << optHeight <<endl;

    return 0;

}
