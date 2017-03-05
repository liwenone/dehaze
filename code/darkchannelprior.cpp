#include "darkchannelprior.h"
#include <QDebug>

void DarkChannelPrior::SetArgs(bool _is_guided, bool _is_sky, bool _is_sample, bool _is_fast, int _min_filter_R, int _guided_filter_R, int _sky_K, int _clahe_clip, int _clahe_size, int _sample)
{
    // 固定参数
    dehaze_factor = 0.95;
    trans_factor = 0.1;
    eps = 0.01;

    // 非去雾算法相关参数，用于控制去雾流程
    is_guided = _is_guided;
    is_sky = _is_sky;
    is_sample = _is_sample;
    is_fast = _is_fast;

    // 去雾相关参数
    min_filter_R = _min_filter_R;
    guided_filter_R = _guided_filter_R;
    sky_K = _sky_K;
    clahe_clip = _clahe_clip;
    clahe_size = _clahe_size;
    sample = _sample;
}

// 获取输入图像
ImgStruct DarkChannelPrior::GetInputImStruct(string file_name) {
    int start = QTime::currentTime().msecsSinceStartOfDay();
    input_img = imread(file_name);
    int end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct input_struct = {end - start, input_img};

    return input_struct;
}

// 获取暗通道图像
Mat DarkChannelPrior::GetDarkChannelImage(Mat input_img, int r) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建暗通道图像
    Mat dark_channel_img(rows, cols, CV_8UC1);
    for (int row = 0; row < rows; row++) {
        uchar* data_input_img = input_img.ptr<uchar>(row); // 原始图像的行像素数据
        uchar* data_dark_channel_img = dark_channel_img.ptr<uchar>(row); // 暗通道的行像素数据
        for (int col = 0; col < cols; col++) {
            // 取 R、G、B三个通道中的最小值
            *(data_dark_channel_img + col) = Tools::getUcharMin(*(data_input_img), Tools::getUcharMin(*(data_input_img + 1), *(data_input_img + 2)));
            data_input_img += 3;
        }
    }

    // 最小值滤波
    int patch_size = 2 * r + 1;
    erode(dark_channel_img, dark_channel_img, getStructuringElement(MORPH_RECT, Size(patch_size, patch_size)));

    return dark_channel_img;
}

// 获取大气光
vector<uchar> DarkChannelPrior::GetAirLight(Mat input_img, Mat dark_channel_img) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 统计亮度为前千分之一的像素点有多少个
    int amount = 0.001 * rows * cols;

    // 统计在暗通道图像中，各个灰度级的像素数量
    int level[256] = {0}; // 数量全部初始化为0
    for (int row = 0; row < rows; row++) {
        uchar* data = dark_channel_img.ptr<uchar>(row);
        for (int col = 0; col < cols; col++) {
            level[data[col]]++;
        }
    }

    // 纪录灰度级中数量不为零的灰度级
    // 因为统计的是最亮的像素点，因此从255（最亮）到0（最暗）统计
    vector<uchar> levels;
    for (int i = 255; i >= 0; i--) {
        if (level[i] != 0) {
            levels.push_back(i);
        }
    }

    // 求暗通道图像中前千分之最暗的像素点的位置
    vector<int> locations;
    for (int i = 0; locations.size() < amount; i++) {
        for (int row = 0; row < rows; row++) {
            if (locations.size() >= amount) break;

            uchar* data = dark_channel_img.ptr<uchar>(row);
            for (int col = 0; col < cols; col++) {
                if (*data == levels[i]) {
                    locations.push_back(row * rows + col); // 位置
                    if (locations.size() >= amount) break;
                }
                data++;
            }
        }
    }

    // 三个通道大气光值
    vector<uchar> A;
    A.push_back(0);
    A.push_back(0);
    A.push_back(0);
    for (int i = 0; i < amount; i++) {
        int row = locations.at(i) / rows;
        int col = locations.at(i) % rows;

        uchar b = input_img.ptr<uchar>(row)[3 * col];
        uchar g = input_img.ptr<uchar>(row)[3 * col + 1];
        uchar r = input_img.ptr<uchar>(row)[3 * col + 2];

        // 取大的值
        if (A[0] < b) A[0] = b;
        if (A[1] < g) A[1] = g;
        if (A[2] < r) A[2] = r;
    }

    return A;
}

// 获取透射图
Mat DarkChannelPrior::GetTransmissionImage(Mat input_img, vector<uchar> A, float factor, int r) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建透射图
    Mat transmission_img(rows, cols, CV_8UC1);
    for (int row = 0; row < rows; row++) {
        uchar* data_input_img = input_img.ptr<uchar>(row);
        uchar* data_transmission_img = transmission_img.ptr<uchar>(row);
        for (int col = 0; col < cols; col++) {
            float value = 1.0f - factor * Tools::getFloatMin(*data_input_img / (float) A[0], Tools::getFloatMin(*(data_input_img + 1) / (float) A[1], *(data_input_img + 2) / (float) A[2]));
            *data_transmission_img = Tools::fixValue(255 * value);

            data_transmission_img++;
            data_input_img += 3;
        }
    }

    // 最大值滤波。因为上面的计算顺序与论文中不一样，论文中的最小值滤波改成最大值滤波
    int patch_size = 2 * r + 1;
    dilate(transmission_img, transmission_img, getStructuringElement(MORPH_RECT, Size(patch_size, patch_size)));

    return transmission_img;
}

// 引导滤波
Mat DarkChannelPrior::GetGuidedFilterImage(Mat I, Mat P, int r, double eps) {
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
Mat DarkChannelPrior::GetHazeFreeImage(Mat input_img, Mat transmission_img, vector<uchar> A, float factor) {
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
            float b = (*data_input_img - A[0]) / Tools::getFloatMax(*data_transmission_img / 255.0f, factor) + A[0];
            float g = (*(data_input_img + 1) - A[1]) / Tools::getFloatMax(*data_transmission_img / 255.0f, factor) + A[1];
            float r = (*(data_input_img + 2) - A[2]) / Tools::getFloatMax(*data_transmission_img / 255.0f, factor) + A[2];

            *data_haze_free_img = Tools::fixValue(b);
            *(data_haze_free_img + 1) = Tools::fixValue(g);
            *(data_haze_free_img + 2) = Tools::fixValue(r);

            data_input_img += 3;
            data_transmission_img++;
            data_haze_free_img += 3;
        }
    }

    return haze_free_img;
}

// 获取无雾图像(天空处理）
Mat DarkChannelPrior::GetHazeFreeImageSky(Mat input_img, Mat transmission_img, vector<uchar> A, float factor) {
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
            float b = Tools::GetTransValue(sky_K, *data_input_img, A[0], *data_transmission_img / 255.0f, factor);
            float g = Tools::GetTransValue(sky_K, *(data_input_img + 1), A[1], *data_transmission_img / 255.0f, factor);
            float r = Tools::GetTransValue(sky_K, *(data_input_img + 2), A[2], *data_transmission_img / 255.0f, factor);

            *data_haze_free_img = Tools::fixValue((*data_input_img - A[0]) / b + A[0]);
            *(data_haze_free_img + 1) = Tools::fixValue((*(data_input_img + 1) - A[1]) / g + A[1]);
            *(data_haze_free_img + 2) = Tools::fixValue((*(data_input_img + 2) - A[2]) / r + A[2]);

            data_input_img += 3;
            data_transmission_img++;
            data_haze_free_img += 3;
        }
    }

    return haze_free_img;
}

// CLAHE 图像增强
Mat DarkChannelPrior::GetCLAHEImage(Mat input_img, int clip, int size) {
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
vector<ImgStruct> DarkChannelPrior::GetAllMats() {
    vector<ImgStruct> mats;
    int start = 0, end = 0;

    // 输入图像
    Mat in;
    if (is_sample) {
        resize(input_img, in, Size(input_img.cols / sample, input_img.rows / sample));
    } else {
        in = input_img;
    }

    // 获取暗通道图
    Mat dark;
    start = QTime::currentTime().msecsSinceStartOfDay();
    if (is_fast) {
       dark = this->GetDarkChannelImageFast(in, min_filter_R);
    } else {
       dark = this->GetDarkChannelImage(in, min_filter_R);
    }
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct dark_struct = {end - start, dark};
    mats.push_back(dark_struct);


    // 获取透射图
    start = QTime::currentTime().msecsSinceStartOfDay();
    vector<uchar> A;
    Mat trans;
    if (is_fast) {
        A = AA;
        trans = this->GetTransmissionImageFast(dark, A[0], dehaze_factor);
    } else {
        A = this->GetAirLight(in, dark);
        trans = this->GetTransmissionImage(in, A, dehaze_factor, min_filter_R);
    }
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct trans_struct = {end - start, trans};
    mats.push_back(trans_struct);


    // 是否进行引导滤波
    Mat T = trans;
    if (is_guided) {
        start = QTime::currentTime().msecsSinceStartOfDay();
        Mat gray_img;
        cvtColor(in, gray_img, CV_BGR2GRAY);
        Mat guide_T = this->GetGuidedFilterImage(gray_img, trans, guided_filter_R, eps);
        end = QTime::currentTime().msecsSinceStartOfDay();
        ImgStruct guide_trans_struct = {end - start, guide_T};
        mats.push_back(guide_trans_struct);
        T = guide_T;
    }

    // 恢复图像
    Mat TT = T;
    if (is_sample) {
        Mat T0;
        resize(T, T0, Size(input_img.cols, input_img.rows));
        TT = T0;
    }

    // 获取无雾图像
    Mat dehaze_img;
    start = QTime::currentTime().msecsSinceStartOfDay();
    if (is_sky) {
        dehaze_img = this->GetHazeFreeImageSky(input_img, TT, A, trans_factor);
    } else {
        dehaze_img = this->GetHazeFreeImage(input_img, TT, A, trans_factor);
    }
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

/*****************************************************************/
// 快速处理
// 获取暗通道图像和大气光
Mat DarkChannelPrior::GetDarkChannelImageFast(Mat input_img, int r) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 大气光
    uchar A = 0;
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
    // 大气光处理
    AA.push_back(A);
    AA.push_back(A);
    AA.push_back(A);

    // 最小值滤波
    int patch_size = 2 * r + 1;
    erode(dark_channel_img, dark_channel_img, getStructuringElement(MORPH_RECT, Size(patch_size, patch_size)));

    return dark_channel_img;
}


// 获取透射图
Mat DarkChannelPrior::GetTransmissionImageFast(Mat dark_img, uchar A, float factor) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建透射图
    Mat transmission_img(rows, cols, CV_8UC1);
    transmission_img = 255 * (1.0f - factor *  dark_img / (float)A);

    return transmission_img;
}
