#include "widget.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":res/icon.png"));
    a.setApplicationName("雾天图像增强");

    Widget w;
    w.show();

    return a.exec();
}
