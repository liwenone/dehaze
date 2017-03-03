#include "widget.h"
#include "stylesheet.h"
#include "darkwidget.h"
#include "fastwidget.h"

#include <QDebug>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    is_title_pressed = false;
    is_maximized = false;


    this->setFixedSize(1024, 710);
    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏

    btn_dark = new QRadioButton();
    btn_dark->setChecked(true);
    btn_dark->setStyleSheet(BTN_DARK);
    btn_fast = new QRadioButton();
    btn_fast->setStyleSheet(BTN_FAST);

    connect(btn_dark, SIGNAL(clicked(bool)), this, SLOT(SlotSelectMethod()));
    connect(btn_fast, SIGNAL(clicked(bool)), this, SLOT(SlotSelectMethod()));

    // left
    widget_dark = new DarkWidget();
    widget_dark->setFixedWidth(310);
    widget_dark->setStyleSheet(WIDGET_LEFT);
    connect(widget_dark, SIGNAL(SignalShowImage(QImage)), this, SLOT(SlotShowImage(QImage)));
    connect(widget_dark, SIGNAL(SignalSaveFile()), this, SLOT(SlotSaveImage()));

    widget_fast = new FastWidget();
    widget_fast->setFixedWidth(310);
    widget_fast->setStyleSheet(WIDGET_LEFT);
    widget_fast->hide();
    connect(widget_fast, SIGNAL(SignalShowImage(QImage)), this, SLOT(SlotShowImage(QImage)));

    // right top
    QWidget* widget_right_top = new QWidget();
    widget_right_top->setFixedHeight(40);
    widget_right_top->setStyleSheet(WIDGET_TOP);

    QHBoxLayout* layout_right_top = new QHBoxLayout();
    layout_right_top->setSpacing(0);
    layout_right_top->setMargin(2);
    layout_right_top->addStretch();
    layout_right_top->addWidget(btn_dark);
    layout_right_top->addWidget(btn_fast);
    widget_right_top->setLayout(layout_right_top);

    // right bottom
    QWidget* widget_right_bottom = new QWidget();
    widget_right_bottom->setStyleSheet(WIDGET_RIGHT);
    label_show = new QLabel(widget_right_bottom);
    label_show->setStyleSheet(LABEL_SHOW_OFF);

    // right
    QVBoxLayout* layout_right = new QVBoxLayout();
    layout_right->addWidget(widget_right_top);
    layout_right->addWidget(widget_right_bottom);

    // main_bottom
    layout_main_bottom = new QHBoxLayout();
    layout_main_bottom->setMargin(0);
    layout_main_bottom->setSpacing(0);
    layout_main_bottom->addWidget(widget_dark);
    layout_main_bottom->addWidget(widget_fast);
    layout_main_bottom->addLayout(layout_right);


    // main_title
    QPushButton* btn_close = new QPushButton();
    QPushButton* btn_min = new QPushButton();
    QPushButton* btn_max = new QPushButton();

    btn_close->setStyleSheet(BNT_CLOSE);
    btn_min->setStyleSheet(BNT_MIN);
    btn_max->setStyleSheet(BNT_MAX);

    connect(btn_close, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(btn_min, SIGNAL(clicked(bool)), this, SLOT(showMinimized()));
    connect(btn_max, SIGNAL(clicked(bool)), this, SLOT(SlotShowMaximized()));

    QHBoxLayout* layout_title = new QHBoxLayout();
    layout_title->addWidget(btn_close);
    layout_title->addWidget(btn_min);
    layout_title->addWidget(btn_max);
    layout_title->addStretch();

    QWidget* widget_title = new QWidget();
    widget_title->setFixedHeight(40);
    widget_title->setStyleSheet(WIDGET_TITLE);
    widget_title->setLayout(layout_title);

    // main
    QVBoxLayout* layout_main = new QVBoxLayout();
    layout_main->setSpacing(0);
    layout_main->setMargin(0);
    layout_main->addWidget(widget_title);
    layout_main->addLayout(layout_main_bottom);

    this->setLayout(layout_main);

}

Widget::~Widget()
{

}

// 窗口切换
void Widget::SlotSelectMethod() {
    label_show->setGeometry(0, 0, 0, 0);

    QObject* btn_sender = this->sender();
    if (btn_sender == btn_dark) {
        widget_dark->show();
        widget_fast->hide();
    } else if (btn_sender == btn_fast) {
        widget_dark->hide();
        widget_fast->show();
    }
}

void Widget::SlotShowImage(QImage img) {
    img_show = img;
    this->ShowImage();
}

// 让图片居中显示
void Widget::ShowImage() {
    QWidget* widget = (QWidget*)label_show->parent();
    int wp = widget->width();
    int hp = widget->height();

    int wimg = img_show.width();
    int himg = img_show.height();

    int x = (wp - wimg) / 2;
    int y = (hp - himg) / 2;

    label_show->setGeometry(x, y, wimg, himg);
    label_show->setStyleSheet(LABEL_SHOW_ON);
    label_show->setPixmap(QPixmap::fromImage(img_show));
}

void Widget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (QRect(QPoint(0, 0), QPoint(this->width(), 40)).contains(event->pos())) {
        this->SlotShowMaximized();
    }
}

void Widget::mousePressEvent(QMouseEvent* event) {
    if (QRect(QPoint(0, 0), QPoint(this->width(), 40)).contains(event->pos()) && !is_maximized && event->button() == Qt::LeftButton) {
        is_title_pressed = true;
        point_start = event->pos();
    }
}

void Widget::mouseMoveEvent(QMouseEvent* event) {
    if (is_title_pressed) {
        point_end = event->pos();
        this->setGeometry(QRect(this->pos() + point_end - point_start, this->pos() + QPoint(this->width(), this->height())));
    }
}

void Widget::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);

    if (is_title_pressed) {
        is_title_pressed = false;
    }
}

void Widget::resizeEvent(QResizeEvent* size) {
    Q_UNUSED(size);
    this->ShowImage();
}

void Widget::SlotShowMaximized() {
    if (is_maximized) {
        is_maximized = false;
        this->showNormal();
    } else {
        is_maximized = true;
        this->showMaximized();
    }
}

void Widget::SlotSaveImage() {

}
