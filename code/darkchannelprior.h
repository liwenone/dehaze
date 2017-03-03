#ifndef DARKCHANNELPRIOR_H
#define DARKCHANNELPRIOR_H

#include "tools.h"

class DarkChannelPrior
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
    int sample; // 采样值

    bool is_guided;
    bool is_sky;
    bool is_sample;
    bool is_fast;

    Mat input_img;

    // 快速处理大气光
    vector<uchar> AA;

public:
    void SetArgs(bool _is_guided, bool _is_sky, bool _is_sample, bool _is_fast, int _min_filter_R, int _guided_filter_R, int _sky_K, int _clahe_clip, int _clahe_size, int _sample);

    // 获取输入图像
    ImgStruct GetInputImStruct(string file_name);

    // 获取暗通道图像
    Mat GetDarkChannelImage(Mat input_img, int mmin_filter_R);

    // 获取大气光
    vector<uchar> GetAirLight(Mat input_img, Mat dark_channel_img);

    // 获取透射图
    Mat GetTransmissionImage(Mat input_img, vector<uchar> A, float factor, int r);

    // 引导滤波
    Mat GetGuidedFilterImage(Mat I, Mat P, int r, double eps);

    // 获取无雾图像
    Mat GetHazeFreeImage(Mat input_img, Mat transmission_img, vector<uchar> A, float factor);

    // 获取无雾图像(天空处理)
    Mat GetHazeFreeImageSky(Mat input_img, Mat transmission_img, vector<uchar> A, float factor);

    // CLAHE 图像增强
    Mat GetCLAHEImage(Mat input_img, int clip, int size);

    // 获取所有的图像
    vector<ImgStruct> GetAllMats();


    /************************************************************************/
    //快速处理对应的方法
    Mat GetDarkChannelImageFast(Mat input_img, int r);

    Mat GetTransmissionImageFast(Mat dark_img, uchar A, float factor);
};

#endif // DARKCHANNELPRIOR_H
