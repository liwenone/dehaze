#ifndef FASTDEHAZE_H
#define FASTDEHAZE_H

#include "tools.h"

class FastDehaze
{
private:
    // 去雾相关参数
    int ave_filter_R;
    float eps;
    int clahe_clip;
    int clahe_size;

    Mat input_img;

public:
    FastDehaze();

    // 设置参数
    void SetArgs(int _ave_filter_R, float _eps, int _clahe_clip, int _clahe_size);

    // 获取输入图像
    ImgStruct GetInputImStruct(string file_name);

    // 获取M
    Mat GetM(Mat input_img);

    // 获取均值滤波后的Mave
    Mat GetMave(Mat input_img, int radius);

    // 获取Mave中所想像素的均值
    float Getm(Mat input_img);

    // 获取大气光
    uchar GetA(Mat input_img, Mat Mave, float eps);

    // 获取环境光图像L
    Mat GetL(Mat M, Mat Mave, float p, float m);

    // 获取无雾图像
    Mat GetDehazeImage(Mat input_img, Mat L, uchar A);

    // CLAHE 图像增强
    Mat GetCLAHEImage(Mat input_img, int clip, int size);

    // 获取所有图像
    vector<ImgStruct> GetAllMats();
};

#endif // FASTDEHAZE_H
