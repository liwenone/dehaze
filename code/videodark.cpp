#include "VideoDark.h"
#include <QDebug>

VideoDark::VideoDark() {
    A = 0;
}

void VideoDark::SetArgs(bool _is_guided, bool _is_sky, int _min_filter_R, int _guided_filter_R, int _sky_K, int _clahe_size)
{

    // 固定参数
    dehaze_factor = 0.95;
    trans_factor = 0.1;
    eps = 0.01;
    clahe_clip = clahe_size;

    // 去雾相关参数
    min_filter_R = _min_filter_R;
    guided_filter_R = _guided_filter_R;
    sky_K = _sky_K;
    clahe_size = 1;
}

// 获取输入图像
ImgStruct VideoDark::GetInputImStruct(string file_name) {
    int start = QTime::currentTime().msecsSinceStartOfDay();
    input_img = imread(file_name);
    int end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct input_struct = {end - start, input_img};

    return input_struct;
}

// 获取暗通道图像和大气光
Mat VideoDark::GetDarkChannelImage(Mat input_img, int r) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建暗通道图像
    Mat dark_channel_img(rows, cols, CV_8UC1);
    for (int row = 0; row < rows; row++) {
        uchar* data_input_img = input_img.ptr<uchar>(row); // 原始图像的行像素数据
        uchar* data_dark_channel_img = dark_channel_img.ptr<uchar>(row); // 暗通道的行像素数据
        for (int col = 0; col < cols; col++) {
            // 取 R、G、B三个通道中的最小值
            uchar c = Tools::getUcharMin(*(data_input_img), Tools::getUcharMin(*(data_input_img + 1), *(data_input_img + 2)));
            *(data_dark_channel_img + col) = c;

            // 大气光估计
            if (c > A) A = c;

            data_input_img += 3;
        }
    }

    // 最小值滤波
    int patch_size = 2 * r + 1;
    erode(dark_channel_img, dark_channel_img, getStructuringElement(MORPH_RECT, Size(patch_size, patch_size)));

    return dark_channel_img;
}


// 获取透射图
Mat VideoDark::GetTransmissionImage(Mat dark_img, uchar A) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建透射图
    Mat transmission_img(rows, cols, CV_8UC1);
    transmission_img = 255 * (1.0f - dark_img / (float)A);

    return transmission_img;
}

// 引导滤波
Mat VideoDark::GetGuidedFilterImage(Mat I, Mat P, int r, double eps) {
    // 规格化图像
    Mat temp_I;
    I.convertTo(temp_I, CV_64FC1, 1.0 / 255.0);
    I = temp_I;

    Mat temp_P;
    P.convertTo(temp_P, CV_64FC1, 1.0 / 255.0);
    P = temp_P;

    int h = I.rows;
    int w = I.cols;

    Mat N;
    boxFilter(cv::Mat::ones(h, w, I.type()), N, CV_64FC1, cv::Size(r, r));


    // 1
    Mat mean_I;
    boxFilter(I, mean_I, CV_64FC1, Size(r, r));

    Mat mean_P;
    boxFilter(P, mean_P, CV_64FC1, cv::Size(r, r));

    Mat corr_I;
    boxFilter(I.mul(I), corr_I, CV_64FC1, Size(r, r));

    Mat corr_IP;
    boxFilter(I.mul(P), corr_IP, CV_64FC1, cv::Size(r, r));

    // 2
    Mat var_I = corr_I - mean_I.mul(mean_I);
    Mat cov_IP = corr_IP - mean_I.mul(mean_P);

    // 3
    Mat a = cov_IP / (var_I + eps);
    Mat b = mean_P - a.mul(mean_I);

    // 4
    Mat mean_a;
    boxFilter(a, mean_a, CV_64FC1, cv::Size(r, r));

    Mat mean_b;
    boxFilter(b, mean_b, CV_64FC1, cv::Size(r, r));

    // 5
    Mat q = mean_a.mul(I) + mean_b;

    // 还原图像
    Mat result_img(q.rows, q.cols, CV_8UC1);
    q.convertTo(result_img, CV_8UC1, 255.0);
    return result_img;
}

// 获取无雾图像
Mat VideoDark::GetHazeFreeImage(Mat input_img, Mat transmission_img, uchar A, float factor) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建无雾图像
    Mat haze_free_img(rows, cols, CV_8UC3);

    for (int row = 0; row < rows; row++) {
        uchar* data_input_img = input_img.ptr<uchar>(row);
        uchar* data_transmission_img = transmission_img.ptr<uchar>(row);
        uchar* data_haze_free_img = haze_free_img.ptr<uchar>(row);
        for (int col = 0; col < cols; col++) {
            // 求取三个通道的颜色值
            float b = Tools::GetTransValue(sky_K, *data_input_img, A, *data_transmission_img / 255.0, factor);
            float g = Tools::GetTransValue(sky_K, *(data_input_img + 1), A, *data_transmission_img / 255.0f, factor);
            float r = Tools::GetTransValue(sky_K, *(data_input_img + 2), A, *data_transmission_img / 255.0f, factor);

            *data_haze_free_img = Tools::fixValue((*data_input_img - A) / b + A);
            *(data_haze_free_img + 1) = Tools::fixValue((*(data_input_img + 1) - A) / g + A);
            *(data_haze_free_img + 2) = Tools::fixValue((*(data_input_img + 2) - A) / r + A);

            data_input_img += 3;
            data_transmission_img++;
            data_haze_free_img += 3;
        }
    }

    return haze_free_img;
}

// CLAHE 图像增强
Mat VideoDark::GetCLAHEImage(Mat input_img, int clip, int size) {
    Mat clahe_img;
    input_img.copyTo(clahe_img);

    // 分离图像的三个通道
    vector<Mat> channels;
    split(clahe_img, channels);

    // 配置CLAHE算法
    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(clip);
    clahe->setTilesGridSize(Size(size, size));

    // 通道一处理
    Mat one;
    clahe->apply(channels.at(0), one);
    channels.at(0) = one;

    // 通道二处理
    Mat two;
    clahe->apply(channels.at(1), two);
    channels.at(1) = two;

    // 通道三处理
    Mat three;
    clahe->apply(channels.at(2), three);
    channels.at(2) = three;

    // 重新合并三个通道
    merge(channels, clahe_img);
    return clahe_img;
}

// 获取所有的图像
vector<ImgStruct> VideoDark::GetAllMats() {
    vector<ImgStruct> mats;
    int start = 0, end = 0;

    Size d = Size(input_img.cols / 4, input_img.rows / 4);
    Mat I;
    resize(input_img, I, d);

    // 获取暗通道图
    start = QTime::currentTime().msecsSinceStartOfDay();
    Mat dark_img = this->GetDarkChannelImage(input_img, min_filter_R);
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct dark_struct = {end - start, dark_img};
    mats.push_back(dark_struct);

    // 获取透射图
    start = QTime::currentTime().msecsSinceStartOfDay();
    Mat trans_img = this->GetTransmissionImage(dark_img, A);
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct trans_struct = {end - start, trans_img};
    mats.push_back(trans_struct);


    // 是否进行引导滤波
    Mat t_img = trans_img;
    start = QTime::currentTime().msecsSinceStartOfDay();
    Mat gray_img;
    cvtColor(input_img, gray_img, CV_BGR2GRAY);
    Mat guide_trans_img = this->GetGuidedFilterImage(gray_img, trans_img, guided_filter_R, eps);
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct guide_trans_struct = {end - start, guide_trans_img};
    mats.push_back(guide_trans_struct);
    t_img = guide_trans_img;


    // 获取无雾图像
    Mat dehaze_img;
    start = QTime::currentTime().msecsSinceStartOfDay();

    //Mat t;
    //resize(t_img, t, Size(t_img.cols * 4, t_img.rows * 4));

    dehaze_img = this->GetHazeFreeImage(input_img, t_img, A, trans_factor);
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct dehaze_struct = {end - start, dehaze_img};
    mats.push_back(dehaze_struct);

    // 获取CLAHE增强后的图像
    start = QTime::currentTime().msecsSinceStartOfDay();
    Mat clahe_img = this->GetCLAHEImage(dehaze_img, clahe_clip, clahe_size);
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct clahe_struct = {end - start, clahe_img};
    mats.push_back(clahe_struct);

    return mats;
}
