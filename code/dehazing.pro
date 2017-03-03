#-------------------------------------------------
#
# Project created by QtCreator 2016-05-19T15:21:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dehazing
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    darkwidget.cpp \
    listmodel.cpp \
    item.cpp \
    itemdelegate.cpp \
    darkchannelprior.cpp \
    tools.cpp \
    fastdehaze.cpp \
    fastwidget.cpp

HEADERS  += widget.h \
    stylesheet.h \
    darkwidget.h \
    listmodel.h \
    item.h \
    itemdelegate.h \
    darkchannelprior.h \
    tools.h \
    fastdehaze.h \
    fastwidget.h

RESOURCES += \
    res.qrc

INCLUDEPATH+=D:\Library\Qt\include\
             D:\Library\Qt\include\opencv2\
             D:\Library\Qt\include\opencv

LIBS+=D:\Library\Qt\lib\libopencv_calib3d2411.dll.a\
        D:\Library\Qt\lib\libopencv_core2411.dll.a\
        D:\Library\Qt\lib\libopencv_features2d2411.dll.a\
        D:\Library\Qt\lib\libopencv_highgui2411.dll.a\
        D:\Library\Qt\lib\libopencv_imgproc2411.dll.a
