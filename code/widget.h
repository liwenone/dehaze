#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include <QMouseEvent>
#include <QResizeEvent>

class Widget : public QWidget
{
    Q_OBJECT

private slots:
    void SlotSelectMethod();
    void SlotShowImage(QImage img);
    void SlotShowMaximized();
    void SlotSaveImage();

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void ShowImage();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void resizeEvent(QResizeEvent* size);

private:
    QRadioButton* btn_dark;
    QRadioButton* btn_fast;

    // 窗口拖动
    bool is_title_pressed;
    QPoint point_start, point_end;

    bool is_maximized;

    // 图片显示
    QLabel* label_show;
    QImage img_show;

    QWidget* widget_dark;
    QWidget* widget_fast;
    QHBoxLayout* layout_main_bottom;
};

#endif // WIDGET_H
