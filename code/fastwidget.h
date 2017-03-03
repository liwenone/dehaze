#ifndef FASTWIDGET_H
#define FASTWIDGET_H


#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QSlider>
#include <QFileDialog>
#include <QPixmap>
#include <QCheckBox>

#include <QList>
#include <QListView>
#include "item.h"
#include "itemdelegate.h"
#include "listmodel.h"

#include "fastdehaze.h"

class FastWidget : public QWidget
{
    Q_OBJECT

private:
    QWidget* widget_title;

    QList<Item> list_items;
    QListView* list_view;
    ListModel* model;
    ItemDelegate* delegate;

    QWidget* widget_control;

    FastDehaze* fast;


    //
    QSlider* slider_ave;
    QSlider* slider_eps;
    QSlider* slider_clip;
    QSlider* slider_constract;
    //
    QLabel* label_ave_value;
    QLabel* label_eps_value;
    QLabel* label_clip_value;
    QLabel* label_constract_value;


    // 控制因子
    int ave_filter_R;
    float eps;
    int clahe_L;
    int clahe_S;

    ImgStruct img_struct;

public:
    explicit FastWidget(QWidget *parent = 0);

    QWidget* CreateTitleWidget();
    QWidget* CreateControlWidget();

    void UpdateListView(ImgStruct img_struct, QString name);
    void ClearList();

signals:
    void SignalShowImage(QImage img);

public slots:
    void SlotOpenFile();
    void SlotDehaze();
    void SlotShowImage(QImage image);
    void SlotSetControlLabelText(int value);
};

#endif // FASTWIDGET_H
