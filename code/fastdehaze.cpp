#include "fastdehaze.h"

#include <QDebug>

FastDehaze::FastDehaze()
{

}

void FastDehaze::SetArgs(int _ave_filter_R, float _eps, int _clahe_clip, int _clahe_size)
{
    // 去雾相关参数
    ave_filter_R = _ave_filter_R;
    eps = _eps;
    clahe_clip = _clahe_clip;
    clahe_size = _clahe_size;
}


// 获取输入图像
ImgStruct FastDehaze::GetInputImStruct(string file_name) {
    int start = QTime::currentTime().msecsSinceStartOfDay();
    input_img = imread(file_name);
    int end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct input_struct = {end - start, input_img};

    return input_struct;
}

// 获取M
Mat FastDehaze::GetM(Mat input_img) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建M，求取三个通道中最小值
    Mat M(rows, cols, CV_8UC1);
    for (int row = 0; row < rows; row++) {
        uchar* data_input_img = input_img.ptr<uchar>(row);
        uchar* data_M = M.ptr<uchar>(row);
        for (int col = 0; col < cols; col++) {
            *data_M = Tools::getUcharMin(*data_input_img, Tools::getUcharMin(*(data_input_img + 1), *(data_input_img + 2)));

            data_M++;
            data_input_img += 3;
        }
    }
    return M;
}

// 获取均值滤波后的Mave
Mat FastDehaze::GetMave(Mat input_img, int radius) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建M，求取三个通道中最小值
    int patch_size = 2 * radius + 1;
    Mat Mave(rows, cols, CV_8UC1);
    blur(input_img, Mave, Size(patch_size, patch_size));

    return Mave;
}

// 获取Mave中所想像素的均值
float FastDehaze::Getm(Mat input_img) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 求所有像素值的和
    long total = 0;
    for (int row = 0; row < rows; row++) {
        uchar* data = input_img.ptr<uchar>(row);
        for (int col = 0; col < cols; col++) {
            total += *data;
            data++;
        }
    }

    // 返回所有像素的均值
    return total / (rows * cols * 255.0f);
}

// 获取大气光
uchar FastDehaze::GetA(Mat input_img, Mat Mave, float eps) {
    int rows = input_img.rows;
    int cols = input_img.cols;


    // 求取输入图像中rgb通道中的最大值，以及均值滤波后的M的像素最大值
    uchar max_input = 0;
    uchar max_Mave = 0;
    for (int row = 0; row < rows; row++) {
        uchar* data_input_img = input_img.ptr<uchar>(row);
        uchar* data_Mave = Mave.ptr<uchar>(row);
        for (int col = 0; col < cols; col++) {
            uchar max = Tools::getUcharMax(*data_input_img, Tools::getUcharMax(*(data_input_img + 1), *(data_input_img + 2)));
            if (max > max_input) max_input = max;
            if (*data_Mave > max_Mave) max_Mave = *data_Mave;

            data_input_img += 3;
            data_Mave++;
        }
    }

    return eps * max_input + (1 - eps) * max_Mave;
}


// 获取环境光图像L
Mat FastDehaze::GetL(Mat M, Mat Mave, float p, float m) {
    int rows = M.rows;
    int cols = M.cols;

    // 求取系数因子
    float factor = Tools::getFloatMin(0.9, p * m);

    // 创建环境光图像
    Mat L(rows, cols, CV_8UC1);
    for (int row = 0; row < rows; row++) {
        uchar* data_M = M.ptr<uchar>(row);
        uchar* data_Mave = Mave.ptr<uchar>(row);
        uchar* data_L = L.ptr<uchar>(row);
        for (int col = 0; col < cols; col++) {
            *data_L = Tools::getUcharMin(factor * (*data_Mave), *data_M);

            data_M++;
            data_Mave++;
            data_L++;
        }
    }

    return L;
}

// 获取无雾图像
Mat FastDehaze::GetDehazeImage(Mat input_img, Mat L, uchar A) {
    int rows = input_img.rows;
    int cols = input_img.cols;

    // 创建无雾图像
    Mat dehaze_img(rows, cols, CV_8UC3);
    for (int row = 0; row < rows; row++) {
        uchar* data_input_img = input_img.ptr<uchar>(row);
        uchar* data_L = L.ptr<uchar>(row);
        uchar* data_dehaze_img = dehaze_img.ptr<uchar>(row);
        for (int col = 0; col < cols; col++) {
            *data_dehaze_img = Tools::fixValue((*data_input_img - *data_L) / (1 - (*data_L /(float) A)));
            *(data_dehaze_img + 1) = Tools::fixValue((*(data_input_img + 1) - *data_L) / (1 - (*data_L /(float) A)));
            *(data_dehaze_img + 2) = Tools::fixValue((*(data_input_img + 2) - *data_L) / (1 - (*data_L / (float) A)));

            data_dehaze_img += 3;
            data_input_img += 3;
            data_L++;
        }
    }

    return dehaze_img;
}

// CLAHE 图像增强
Mat FastDehaze::GetCLAHEImage(Mat input_img, int clip, int size) {
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

// 获取所有图像
vector<ImgStruct> FastDehaze::GetAllMats() {
    vector<ImgStruct> mats;
    int start = 0, end = 0;

    // 获取RGB最小分量图
    start = QTime::currentTime().msecsSinceStartOfDay();
    Mat M = this->GetM(input_img);
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct M_struct = {end - start, M};
    mats.push_back(M_struct);

    // 获取均值滤波图
    start = QTime::currentTime().msecsSinceStartOfDay();
    Mat Mave = this->GetMave(M, ave_filter_R);
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct Mave_struct = {end - start, Mave};
    mats.push_back(Mave_struct);

    // 获取环境光图像
    start = QTime::currentTime().msecsSinceStartOfDay();
    float m = this->Getm(M);
    float p = 1.28;
    Mat L = this->GetL(M, Mave, p, m);
    end = QTime::currentTime().msecsSinceStartOfDay();
    ImgStruct L_struct = {end - start, L};
    mats.push_back(L_struct);

    // 获取无雾图像
    start = QTime::currentTime().msecsSinceStartOfDay();
    uchar A = this->GetA(input_img, Mave, eps);
    Mat dehaze_img = this->GetDehazeImage(input_img, L, A);
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
