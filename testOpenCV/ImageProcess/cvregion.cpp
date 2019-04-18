#include "cvregion.h"


Region::Region()
{
}
Region::~Region()
{

}
Region::Region(Mat& mat, const cv::Rect& box, const Point2d& center_point, const int& Area)
{
    genRunCode(mat, box, this->region_codes);
    this->Mat_size = mat.size();
    this->bounding_box = box;
    this->center_point = center_point;
    this->area = Area;
}
int Region:: getWidth() const
{
    return this->bounding_box.width;
}
int Region:: getHeight() const
{
    return this->bounding_box.height;
}
int Region:: getArea() const
{
    return this->area;
}

int Region::getBeginColumn() const
{
    return this->bounding_box.tl().x;
}
int Region::getBeginRow() const
{
    return this->bounding_box.tl().y;
}

void cv_threshold(Mat& srcImg, Mat& outImg, double lowVal, double hightVal)
{
    if(srcImg.channels()!=1)
    {
        throw "阈值化操作需要为灰度图";
    }
    Mat lowImg;
    threshold(srcImg, lowImg, lowVal, 255, cv::THRESH_BINARY);
    Mat hightImg;
    threshold(srcImg, hightImg, hightVal, 255, cv::THRESH_BINARY_INV);
    bitwise_and(lowImg, hightImg, outImg);
}
void connection(Mat& binaryImg, std::vector<Region>& out_regions)
{
    try
    {
//        vector<Region> regions;
        out_regions.clear();  //clear 擦除元素，如果元素是指针，那么指向的那块内存会不可访问
        Mat labels, stats, centroids;
        int nccomps = connectedComponentsWithStats(binaryImg, labels, stats, centroids);
        std::vector<std::vector<Point>> vec_vecOfPoint(nccomps-1); //初始化nccomps-1个点集

        //使用std::vector<Mat> region_mats;的话临时内存损耗太大了，会卡主
//        //不能用std::vector<Mat> region_mats(n, Mat::zeros(binaryImg.size(), CV_8U);
//        //因为这样的话全部会初始化成为同一个对象
//        //label 0 为背景标签
        for(int row_i = 0 ; row_i< binaryImg.rows; ++row_i)
        {
            for(int col_j = 0; col_j < binaryImg.cols; ++col_j)
            {
                int label = labels.at<int>(row_i, col_j);
                CV_Assert(0 <= label && label <= nccomps);
                if(label) //label为0的时候为背景
                {
                    cv::Point temp_Point(col_j, row_i);
                    vec_vecOfPoint[label-1].push_back(std::move(temp_Point));//存入点集合
                }

            }
        }
        for(int i = 1; i < nccomps; ++i) //
        {
            int width = stats.at<int>(i, cv::CC_STAT_WIDTH);
            int height = stats.at<int>(i, cv::CC_STAT_HEIGHT);
            int cord_x =  stats.at<int>(i, cv::CC_STAT_LEFT);
            int  cord_y =  stats.at<int>(i, cv::CC_STAT_TOP);
            int area = stats.at<int>(i, cv::CC_STAT_AREA);
            Rect box = Rect(cord_x, cord_y, width, height);
            Point2f center_point = Point2d(centroids.at<int>(i, 0), centroids.at<int>(i, 1));
            Mat tmp_img = Mat::zeros(binaryImg.size(), CV_8U);
            for(Point &p : vec_vecOfPoint[i-1])
            {
                tmp_img.at<uchar>(p.y, p.x) = 255;
            }
//            imshow("region", tmp_img);
//            waitKey(-1);
            Region tm_region(tmp_img, box, center_point, area);
            //这里将拷贝变成移动
            out_regions.push_back(std::move(tm_region));
//            regions.push_back(tm_region);
        }
        //=是拷贝复制
//        out_regions = regions;

    }catch(cv::Exception e)
    {
        cout << "error";
    }

}
void genRunCode(const Mat& input_mat, vector<run_code>& out_run_codes)
{
    vector<run_code> run_codes;
    int current_code_index = 0;
    for(int row_i = 0 ; row_i< input_mat.rows; ++row_i)
    {
        for(int col_j = 0; col_j < input_mat.cols; ++col_j)
        {
            if(input_mat.at<uchar>(row_i,col_j)) //添加第一条行程
            {
                if(!run_codes.empty())
                {
                  if(row_i == run_codes[current_code_index].row)
                  {
                      if(run_codes[current_code_index].endColum == col_j-1)
                      {
                          run_codes[current_code_index].endColum = col_j;
                      }
                      else
                      {
                          run_codes.push_back(run_code{row_i, col_j, col_j}); //添加同一行的下一条行程
                          current_code_index += 1;                   //行程指向下一个
                      }
                  }
                  else   //换行新增行程
                  {
                      run_codes.push_back(run_code{row_i, col_j, col_j});
                      current_code_index += 1;
                  }
                }
                else
                {
                    run_codes.push_back(run_code{row_i, col_j, col_j}); //添加第一条行程
                }

            }

        }
    }
    out_run_codes = run_codes;
}
void genRunCode(const Mat& input_mat, const cv::Rect& bounding_box, vector<run_code>& out_run_codes)
{
    vector<run_code> run_codes;
    int current_code_index = 0;
    for(int row_i = bounding_box.tl().y; row_i< bounding_box.br().y; ++row_i)
    {
        for(int col_j = bounding_box.tl().x; col_j <bounding_box.br().x; ++col_j)
        {
            if(input_mat.at<uchar>(row_i,col_j)) //添加第一条行程
            {
                if(!run_codes.empty())
                {
                  if(row_i == run_codes[current_code_index].row)
                  {
                      if(run_codes[current_code_index].endColum == col_j-1)
                      {
                          run_codes[current_code_index].endColum = col_j;
                      }
                      else
                      {
                          run_codes.push_back(run_code{row_i, col_j, col_j}); //添加同一行的下一条行程
                          current_code_index += 1;                   //行程指向下一个
                      }
                  }
                  else   //换行新增行程
                  {
                      run_codes.push_back(run_code{row_i, col_j, col_j});
                      current_code_index += 1;
                  }
                }
                else
                {
                    run_codes.push_back(run_code{row_i, col_j, col_j}); //添加第一条行程
                }

            }

        }
    }
    out_run_codes = run_codes;
}
void genMatFromRunCode( vector<run_code>& out_run_codes, Mat& out_mat, cv::Size sz)
{
    Mat tmp_mat = Mat::zeros(sz, CV_8U);
    for(run_code& r: out_run_codes)
    {
        for (int col_i = r.beginColum; col_i <= r.endColum; ++col_i )
        {
            tmp_mat.at<uchar>(r.row, col_i) = 255;
        }

    }
    out_mat = tmp_mat.clone();
}

void sort_regions_index(vector<Region>& regions,RegionFeature feature,
                        vector<int>& sortIdx, bool is_reverse)
{
    sortIdx.resize(regions.size(),0);
    int index = 0;
    //给sorIdx赋值
    transform(sortIdx.begin(),sortIdx.end(), sortIdx.begin(),
              [&index](int i) -> int { return index++; });
    switch (feature) {
    case Area:
        std::sort(sortIdx.begin(),sortIdx.end(),
             [&regions](const int& a, const int& b)
                {return regions[a].getArea() < regions[b].getArea();});
        break;
    case Width:
        std::sort(sortIdx.begin(),sortIdx.end(),
             [&regions](const int& a, const int& b)
                {return regions[a].getWidth() < regions[b].getWidth();});
        break;
    case Height:
        std::sort(sortIdx.begin(),sortIdx.end(),
             [&regions](const int& a, const int& b)
            {return regions[a].getHeight() < regions[b].getHeight();});
        break;
    case Row:
        std::sort(sortIdx.begin(),sortIdx.end(),
             [&regions](const int& a, const int& b)
            {return regions[a].getBeginRow() < regions[b].getBeginRow();});
        break;
    case Column:
        std::sort(sortIdx.begin(),sortIdx.end(),
             [&regions](const int& a, const int& b)
            {return regions[a].getBeginColumn() < regions[b].getBeginColumn();});
        break;
    default:
        break;
    }
    //获取feature排列 index

    if(is_reverse)
    {
        std::reverse(sortIdx.begin(), sortIdx.end());
    }

}

//调用一个const对象的非const成员函数这是不允许的
void sort_regions(vector<Region> &regions, RegionFeature feature,
                  vector<Region>& sortedRegions, bool is_reverse)
{
    //重新分配size
    sortedRegions = regions;

    switch (feature) {
    case Area:
        std::sort(sortedRegions.begin(), sortedRegions.end(),
                  [](const Region& a, const Region& b)
                    { return a.getArea() < b.getArea();});
        break;
    case Width:
        std::sort(sortedRegions.begin(), sortedRegions.end(),
                  [](const Region& a, const Region& b)
                    { return a.getWidth() < b.getWidth();});
        break;
    case Height:
        std::sort(sortedRegions.begin(), sortedRegions.end(),
                  []( const Region& a, const Region& b)
                    { return a.getHeight() < b.getHeight();});
        break;
    case Row:
        std::sort(sortedRegions.begin(), sortedRegions.end(),
                  []( const Region& a, const Region& b)
                    { return a.getBeginRow() < b.getBeginRow();});
        break;
    case Column:
        std::sort(sortedRegions.begin(), sortedRegions.end(),
                  []( const Region& a, const Region& b)
                    { return a.getBeginColumn() < b.getBeginColumn();});
        break;
    default:
        break;
    }
    if(is_reverse)
    {
        std::reverse(sortedRegions.begin(), sortedRegions.end());
    }

}

void select_region_shape(vector<Region> &regions, RegionFeature feature,
                         vector<Region>& select_regions, float minVal, float maxVal)
{
    select_regions.resize(regions.size());
    vector<Region> _selectedRegions;
    switch (feature) {
    case Area:
        for_each(regions.begin(), regions.end(),
                 [&](Region& _region)
                  {if(_region.getArea() >= minVal && _region.getArea() <= maxVal)
                     _selectedRegions.push_back(_region);});

        break;
    case Width:
        for_each(regions.begin(), regions.end(),
                 [&](Region& _region)
                  {if(_region.getWidth() >= minVal && _region.getWidth() <= maxVal)
                     _selectedRegions.push_back(_region);});
        break;
    case Height:
        for_each(regions.begin(), regions.end(),
                 [&](Region& _region)
                  {if(_region.getHeight() >= minVal && _region.getHeight() <= maxVal)
                     _selectedRegions.push_back(_region);});
        break;
    case Row:
        for_each(regions.begin(), regions.end(),
                 [&](Region& _region)
                  {if(_region.getBeginRow() >= minVal && _region.getBeginRow() <= maxVal)
                     _selectedRegions.push_back(_region);});
        break;
    case Column:
        for_each(regions.begin(), regions.end(),
                 [&](Region& _region)
                  {if(_region.getBeginColumn() >= minVal && _region.getBeginColumn() <= maxVal)
                     _selectedRegions.push_back(_region);});
        break;
    default:
        break;
    }
    select_regions = _selectedRegions;

}

void frequence_blur()
{
    Mat input=imread("/home/lilanluo/image.bmp", 0);

    imshow("input",input);
    int w=getOptimalDFTSize(input.cols);
    int h=getOptimalDFTSize(input.rows);
    Mat padded;
    copyMakeBorder(input,padded,0,h-input.rows,0,w-input.cols,BORDER_CONSTANT,Scalar::all(0));
    padded.convertTo(padded,CV_32FC1);
    imshow("padded",padded);
    for(int i=0;i<padded.rows;i++)//中心化操作，其余操作和上一篇博客的介绍一样
    {
        float *ptr=padded.ptr<float>(i);
        for(int j=0;j<padded.cols;j++)	ptr[j]*=pow(-1,i+j);
    }
    Mat plane[]={padded,Mat::zeros(padded.size(),CV_32F)};
    Mat complexImg;
    merge(plane,2,complexImg);
    dft(complexImg,complexImg);
//************************gaussian****************************
    Mat gaussianBlur(padded.size(),CV_32FC2);
    Mat gaussianSharpen(padded.size(),CV_32FC2);
    float D0=2*10*10;
    for(int i=0;i<padded.rows;i++)
    {
        float*p=gaussianBlur.ptr<float>(i);
        float*q=gaussianSharpen.ptr<float>(i);
        for(int j=0;j<padded.cols;j++)
        {
            float d=pow(i-padded.rows/2,2)+pow(j-padded.cols/2,2);
            p[2*j]=expf(-d/D0);
            p[2*j+1]=expf(-d/D0);

            q[2*j]=1-expf(-d/D0);
            q[2*j+1]=1-expf(-d/D0);
        }
    }
    multiply(complexImg,gaussianBlur,gaussianBlur);//矩阵元素对应相乘法，注意，和矩阵相乘区分
    multiply(complexImg,gaussianSharpen,gaussianSharpen);
//*****************************************************************
    split(complexImg,plane);
    magnitude(plane[0],plane[1],plane[0]);
    plane[0]+=Scalar::all(1);
    log(plane[0],plane[0]);
    normalize(plane[0],plane[0],1,0,CV_MINMAX);
    imshow("dft",plane[0]);
//******************************************************************
    split(gaussianBlur,plane);
    magnitude(plane[0],plane[1],plane[0]);
    plane[0]+=Scalar::all(1);
    log(plane[0],plane[0]);
    normalize(plane[0],plane[0],1,0,CV_MINMAX);
    imshow("gaussianBlur",plane[0]);

    split(gaussianSharpen,plane);
    magnitude(plane[0],plane[1],plane[0]);
    plane[0]+=Scalar::all(1);
    log(plane[0],plane[0]);
    normalize(plane[0],plane[0],1,0,CV_MINMAX);
    imshow("gaussianSharpen",plane[0]);
//******************************************************************
//*************************idft*************************************
    idft(gaussianBlur,gaussianBlur);
    idft(gaussianSharpen,gaussianSharpen);
    split(gaussianBlur,plane);
    magnitude(plane[0],plane[1],plane[0]);
    normalize(plane[0],plane[0],1,0,CV_MINMAX);
    imshow("idft-gaussianBlur",plane[0]);

    split(gaussianSharpen,plane);
    magnitude(plane[0],plane[1],plane[0]);
    normalize(plane[0],plane[0],1,0,CV_MINMAX);
    imshow("idft-gaussianSharpen",plane[0]);

    waitKey();
}

