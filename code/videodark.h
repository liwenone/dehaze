#ifndef VIDEODARK_H
#define VIDEODARK_H

#include "tools.h"

class VideoDark
{
private:
    int min_filter_R; // 最小值滤波半径
    float dehaze_factor; // 去雾因子
    float trans_factor; // 透射图最小值
    int guided_filter_R; // 引导滤波半径
    float eps; // 引导滤波修正因子
    int clahe_clip; // CLAHE剪裁限幅
    int clahe_size; // 分块大小
    int sky_K; // 天空区域容差值

    Mat input_img;

    // 大气光
    uchar A;

public:
    VideoDark();

    void SetArgs(bool _is_guided, bool _is_sky, int _min_filter_R, int _guided_filter_R, int _sky_K, int _clahe_size);

    // 获取输入图像
    ImgStruct GetInputImStruct(string file_name);

    // 获取暗通道图像和大气光
    Mat GetDarkChannelImage(Mat input_img, int mmin_filter_R);


    // 获取透射图
    Mat GetTransmissionImage(Mat dark_img, uchar A);

    // 引导滤波
    Mat GetGuidedFilterImage(Mat I, Mat P, int r, double eps);

    // 获取无雾图像
    Mat GetHazeFreeImage(Mat input_img, Mat transmission_img, uchar A, float factor);

    // CLAHE 图像增强
    Mat GetCLAHEImage(Mat input_img, int clip, int size);


    // 获取所有的图像
    vector<ImgStruct> GetAllMats();
};

#endif // VIDEODARK_H
