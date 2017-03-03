#include "tools.h"

#include <QDebug>

Tools::Tools()
{

}

uchar Tools::getUcharMin(uchar a, uchar b) {
    return  a < b ? a : b;
}

uchar Tools::getUcharMax(uchar a, uchar b) {
    return  a > b ? a : b;
}

float Tools::getFloatMin(float a, float b) {
    return a < b ? a : b;
}

float Tools::getFloatMax(float a, float b) {
    return a > b ? a : b;
}

uchar Tools::fixValue(float value) {
    if (value > 255.0) {
        value = 255;
    } else if (value < 0.0) {
        value = 0;
    }
    return value;
}

float Tools::GetTransValue(int k, uchar I, uchar A, float t, float t0) {
    int a = Tools::abs(I - A);
    if (a == 0) return 1;

    float b = Tools::getFloatMax(k / (float)a, 1.0);
    float c = b * Tools::getFloatMax(t, t0);

    return Tools::getFloatMin(c, 1.0);
}

int Tools::abs(int value) {
    return value > 0 ? value : -value;
}

// 将OpenCV的矩阵转换为Qt的图像
QImage Tools::ConverMatToImage(const Mat &mat) {
    QImage image;
    switch(mat.type()) {
    case CV_8UC4:
        image = QImage( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32 );
        return image.convertToFormat(QImage::Format_RGB888);
    case CV_8UC3:
        image = QImage( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888 );
        return image.rgbSwapped();
    case CV_8UC1:
        static QVector<QRgb>  sColorTable;
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }
        image = QImage( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8 );
        image.setColorTable( sColorTable );
        return image.convertToFormat(QImage::Format_RGB888);
    default:
        break;
    }

    return QImage();
}
