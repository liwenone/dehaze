#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QImage>
#include <QTime>

using namespace std;
using namespace cv;

// 用于保存图像矩阵和时间的结构体
typedef struct {
    int time;
    Mat mat;
} ImgStruct ;

typedef struct {
    int row;
    int col;
} point_struct;


class Tools
{
public:
    Tools();

    static uchar getUcharMax(uchar a, uchar b);
    static uchar getUcharMin(uchar a, uchar b);
    static float getFloatMin(float a, float b);
    static float getFloatMax(float a, float b);

    static uchar fixValue(float value);
    static int abs(int value);

    static float GetTransValue(int k, uchar I, uchar A, float t, float t0);

    static QImage ConverMatToImage(const Mat &input_mat);
};

#endif // TOOLS_H
