#ifndef DARKWIDGET_H
#define DARKWIDGET_H

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

#include "darkchannelprior.h"

class DarkWidget : public QWidget
{
    Q_OBJECT

private:
    QWidget* widget_title;

    QList<Item> list_items;
    QListView* list_view;
    ListModel* model;
    ItemDelegate* delegate;

    QWidget* widget_control;
    QWidget* widget_switch;

    DarkChannelPrior* dark;

    //
    QSlider* slider_min;
    QSlider* slider_clip;
    QSlider* slider_constract;
    QSlider* slider_guide;
    QSlider* slider_airlight;
    QSlider* slider_sample;
    //
    QLabel* label_min_value;
    QLabel* label_clip_value;
    QLabel* label_constract_value;
    QLabel* label_guide_value;
    QLabel* label_airlight_value;
    QLabel* label_sample_value;


    // 控制因子
    int min_filter_R;
    int guide_filter_R;
    int sky_K;
    int sample;
    int clahe_L;
    int clahe_S;

    bool is_guide;
    bool is_sky;
    bool is_fast;
    bool is_sample;

    ImgStruct img_struct;

public:
    explicit DarkWidget(QWidget *parent = 0);

    QWidget* CreateTitleWidget();
    QWidget* CreateControlWidget();
    QWidget* CreateSwitchWidget();

    void UpdateListView(ImgStruct img_struct, QString name);
    void ClearList();

signals:
    void SignalShowImage(QImage img);
    void SignalSaveFile();

public slots:
    void SlotOpenFile();
    void SlotSaveFile();
    void SlotDehaze();
    void SlotShowImage(QImage image);
    void SlotSetControlLabelText(int value);

    void SlotSwitchGuide(bool is_open);
    void SlotSwitchSky(bool is_open);
    void SlotSwitchSample(bool is_open);
    void SlotSwitchFast(bool is_open);
};

#endif // DARKWIDGET_H
