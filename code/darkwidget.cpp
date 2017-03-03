#include "darkwidget.h"
#include "stylesheet.h"

#include <QDebug>

DarkWidget::DarkWidget(QWidget *parent) : QWidget(parent)
{
    // 去雾参数
    min_filter_R = 7;
    guide_filter_R = 4 * 7;
    sky_K = 50;
    sample = 2;
    clahe_L = 2;
    clahe_S = 2;

    dark = NULL;

    // 去雾算法分支选择
    is_guide = true;
    is_sky = true;
    is_sample = false;
    is_fast = false;


    // 创建标题栏
    widget_title = CreateTitleWidget();

    // 创建条目栏
    list_view = new QListView(this);
    list_view->setStyleSheet(LIST_VIEW);
    list_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    delegate = new ItemDelegate(this);
    list_view->setItemDelegate(delegate);
    connect(delegate, SIGNAL(SignalShowImage(QImage)), this, SLOT(SlotShowImage(QImage)));

    // 创建控制栏
    widget_control = CreateControlWidget();
    widget_switch = CreateSwitchWidget();

    // 主布局
    QVBoxLayout* layout_main = new QVBoxLayout();
    layout_main->setMargin(0);
    layout_main->setSpacing(0);
    layout_main->addWidget(widget_title);
    layout_main->addWidget(list_view);
    layout_main->addWidget(widget_control);
    layout_main->addWidget(widget_switch);

    this->setLayout(layout_main);
}

// 创建标题栏
QWidget* DarkWidget::CreateTitleWidget() {
    QWidget* widget = new QWidget();
    widget->setStyleSheet(WIDGET_LEFT_TOP);
    widget->setFixedHeight(90);

    QLabel* label_title = new QLabel("暗通道去雾");
    label_title->setStyleSheet(LABEL_TITLE);
    label_title->setFixedHeight(40);

    QPushButton* btn_open = new QPushButton("打开");
    QPushButton* btn_save = new QPushButton("保存");
    QPushButton* btn_dehaze = new QPushButton("去雾");
    btn_open->setStyleSheet(BTN_TITLE);
    btn_save->setStyleSheet(BTN_TITLE);
    btn_dehaze->setStyleSheet(BTN_TITLE);

    connect(btn_open, SIGNAL(clicked(bool)), this, SLOT(SlotOpenFile()));
    connect(btn_save, SIGNAL(clicked(bool)), this, SLOT(SlotSaveFile()));
    connect(btn_dehaze, SIGNAL(clicked(bool)), this, SLOT(SlotDehaze()));

    QHBoxLayout* layout_btn = new QHBoxLayout();
    layout_btn->setMargin(5);
    layout_btn->addWidget(btn_open);
    layout_btn->addWidget(btn_save);
    layout_btn->addWidget(btn_dehaze);
    layout_btn->addStretch();

    QVBoxLayout* layout_main = new QVBoxLayout();
    layout_main->setMargin(0);
    layout_main->addWidget(label_title);
    layout_main->addLayout(layout_btn);
    layout_main->addStretch();

    widget->setLayout(layout_main);
    return widget;
}

// 创建控制栏
QWidget* DarkWidget::CreateControlWidget() {
    QWidget* widget_control = new QWidget();


    // 控制因素标签
    QLabel* label_min = new QLabel("最小值滤波半径：");
    QLabel* label_clip = new QLabel("CLAHE剪裁限幅：");
    QLabel* label_constract = new QLabel("CLAHE块大小：");
    QLabel* label_guide = new QLabel("导向滤波半径：");
    QLabel* label_airlight = new QLabel("天空区域容差：");
    QLabel* label_sample = new QLabel("采样系数：");

    // 将控制因素标签添加到布局
    QVBoxLayout* layout_left = new QVBoxLayout();
    layout_left->addWidget(label_min);
    layout_left->addWidget(label_clip);
    layout_left->addWidget(label_constract);
    layout_left->addWidget(label_guide);
    layout_left->addWidget(label_airlight);
    layout_left->addWidget(label_sample);

    // 控制值
    label_min_value = new QLabel();
    label_clip_value = new QLabel();
    label_constract_value = new QLabel();
    label_guide_value = new QLabel();
    label_airlight_value = new QLabel();
    label_sample_value = new QLabel();
    label_min_value->setFixedWidth(23);
    // 添加到布局
    QVBoxLayout* layout_right = new QVBoxLayout();
    layout_right->addWidget(label_min_value);
    layout_right->addWidget(label_clip_value);
    layout_right->addWidget(label_constract_value);
    layout_right->addWidget(label_guide_value);
    layout_right->addWidget(label_airlight_value);
    layout_right->addWidget(label_sample_value);

    // 控制滑块
    slider_min = new QSlider(Qt::Horizontal);
    slider_clip = new QSlider(Qt::Horizontal);
    slider_constract = new QSlider(Qt::Horizontal);
    slider_guide = new QSlider(Qt::Horizontal);
    slider_airlight = new QSlider(Qt::Horizontal);
    slider_sample = new QSlider(Qt::Horizontal);

    slider_min->setStyleSheet(SLIDER);
    slider_constract->setStyleSheet(SLIDER);
    slider_clip->setStyleSheet(SLIDER);
    slider_guide->setStyleSheet(SLIDER);
    slider_airlight->setStyleSheet(SLIDER);
    slider_sample->setStyleSheet(SLIDER);
    slider_sample->setEnabled(false);

    connect(slider_min, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));
    connect(slider_clip, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));
    connect(slider_constract, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));
    connect(slider_guide, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));
    connect(slider_airlight, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));
    connect(slider_sample, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));

    // 初始化滑块的值
    slider_min->setValue(min_filter_R);
    slider_min->setMinimum(1);
    slider_min->setMaximum(100);

    slider_clip->setValue(clahe_L);
    slider_clip->setMinimum(1);
    slider_clip->setMaximum(100);

    slider_constract->setValue(clahe_S);
    slider_constract->setMinimum(1);
    slider_constract->setMaximum(100);

    slider_guide->setValue(guide_filter_R);
    slider_guide->setMinimum(1);
    slider_guide->setMaximum(200);

    slider_airlight->setValue(sky_K);
    slider_airlight->setMinimum(0);
    slider_airlight->setMaximum(200);

    slider_sample->setValue(sample);
    slider_sample->setMinimum(1);
    slider_sample->setMaximum(10);


    // 将控制滑块添加到布局
    QVBoxLayout* layout_center = new QVBoxLayout();
    layout_center->addWidget(slider_min);
    layout_center->addWidget(slider_clip);
    layout_center->addWidget(slider_constract);
    layout_center->addWidget(slider_guide);
    layout_center->addWidget(slider_airlight);
    layout_center->addWidget(slider_sample);

    QHBoxLayout* layout_main = new QHBoxLayout();
    layout_main->addLayout(layout_left);
    layout_main->addLayout(layout_center);
    layout_main->addLayout(layout_right);

    widget_control->setLayout(layout_main);

    return widget_control;
}

// 创建算法开关栏
QWidget* DarkWidget::CreateSwitchWidget() {
    QWidget* widget_switch = new QWidget();
    widget_switch->setStyleSheet(WIDGET_SWITCH);

    // guide
    QLabel* label_guide = new QLabel("导向滤波");
    QCheckBox* box_guide = new QCheckBox();
    box_guide->setStyleSheet(BOX_SWITCH);
    box_guide->setFixedSize(20, 20);
    box_guide->setCheckable(true);
    connect(box_guide, SIGNAL(toggled(bool)), this, SLOT(SlotSwitchGuide(bool)));
    box_guide->setChecked(true);

    QHBoxLayout* layout_guide = new QHBoxLayout();
    layout_guide->setSpacing(0);
    layout_guide->setMargin(0);
    layout_guide->addWidget(box_guide);
    layout_guide->addWidget(label_guide);
    layout_guide->addStretch();

    // sky
    QLabel* label_sky = new QLabel("天空处理");
    QCheckBox* box_sky = new QCheckBox();
    box_sky->setStyleSheet(BOX_SWITCH);
    box_sky->setFixedSize(20, 20);
    box_sky->setCheckable(true);
    connect(box_sky, SIGNAL(toggled(bool)), this, SLOT(SlotSwitchSky(bool)));
    box_sky->setChecked(true);

    QHBoxLayout* layout_sky = new QHBoxLayout();
    layout_sky->setSpacing(0);
    layout_sky->setMargin(0);
    layout_sky->addWidget(box_sky);
    layout_sky->addWidget(label_sky);
    layout_sky->addStretch();


    // sample
    QLabel* label_sample = new QLabel("图像采样");
    QCheckBox* box_sample = new QCheckBox();
    box_sample->setStyleSheet(BOX_SWITCH);
    box_sample->setFixedSize(20, 20);
    connect(box_sample, SIGNAL(toggled(bool)), this, SLOT(SlotSwitchSample(bool)));
    box_sample->setChecked(false);

    QHBoxLayout* layout_sample = new QHBoxLayout();
    layout_sample->setSpacing(0);
    layout_sample->setMargin(0);
    layout_sample->addWidget(box_sample);
    layout_sample->addWidget(label_sample);
    layout_sample->addStretch();

    // 快速处理
    QLabel* label_fast = new QLabel("快速处理");
    QCheckBox* box_fast = new QCheckBox();
    box_fast->setStyleSheet(BOX_SWITCH);
    box_fast->setFixedSize(20, 20);
    connect(box_fast, SIGNAL(toggled(bool)), this, SLOT(SlotSwitchFast(bool)));
    box_fast->setChecked(false);

    QHBoxLayout* layout_fast = new QHBoxLayout();
    layout_fast->setSpacing(0);
    layout_fast->setMargin(0);
    layout_fast->addWidget(box_fast);
    layout_fast->addWidget(label_fast);
    layout_fast->addStretch();

    // main
    QHBoxLayout* layout_main = new QHBoxLayout();
    layout_main->addLayout(layout_guide);
    layout_main->addLayout(layout_sky);
    layout_main->addLayout(layout_sample);
    layout_main->addLayout(layout_fast);

    widget_switch->setLayout(layout_main);

    return widget_switch;
}

// 打开文件，准备输入图像
void DarkWidget::SlotOpenFile() {
    QString file_name = QFileDialog::getOpenFileName(this, "选择图片", "./Pictures", "image files(*.png *.jpeg *.jpg)");
    if (file_name.isEmpty() || file_name.isNull()) {
        qDebug() << "打开失败";
        return;
    }

    // 清理list
    list_items.clear();
    model = new ListModel(list_items);
    list_view->setModel(model);

    if (dark != NULL) {
        delete dark;
    }
    dark = new DarkChannelPrior();
    img_struct = dark->GetInputImStruct(file_name.toStdString());
    this->ClearList();
}


// 清理左侧列表，防止列表中的元素重复出现
void DarkWidget::ClearList() {
    // 清理list
    list_items.clear();
    delete model;

    this->UpdateListView(img_struct, "输入图像");

    QImage img = Tools::ConverMatToImage(img_struct.mat);
    emit SignalShowImage(img);
}

void DarkWidget::SlotSaveFile() {
    emit SignalSaveFile();
}

// 去雾
void DarkWidget::SlotDehaze() {
    if (dark != NULL) {
        this->ClearList();
        dark->SetArgs(is_guide, is_sky, is_sample, is_fast, min_filter_R, guide_filter_R, sky_K, clahe_L, clahe_S, sample);
        int i = 0;
        vector<ImgStruct> mats = dark->GetAllMats();
        this->UpdateListView(mats.at(i++), "暗通道图");
        this->UpdateListView(mats.at(i++), "透射图");
        if (is_guide) {
            this->UpdateListView(mats.at(i++), "导向滤波透射图");
        }
        this->UpdateListView(mats.at(i++), "无雾图像");
        this->UpdateListView(mats.at(i++), "色阶增强无雾图像");
    }
}

// 更新左侧列表，重新显示条目
void DarkWidget::UpdateListView(ImgStruct img_struct, QString name) {
    QImage img = Tools::ConverMatToImage(img_struct.mat);

    Item item(img, name, img_struct.time);
    list_items.append(item);

    model = new ListModel(list_items);
    list_view->setModel(model);
}

void DarkWidget::SlotShowImage(QImage image) {
     emit SignalShowImage(image);
}

// 联动更新控制标签的文字
void DarkWidget::SlotSetControlLabelText(int value) {
    QString text = QString::number(value);

    QSlider* slider = (QSlider*)this->sender();
    if (slider == slider_min) {
        label_min_value->setText(text);
        min_filter_R = value;
    } else if (slider == slider_guide) {
        label_guide_value->setText(text);
        guide_filter_R = value;
    } else if (slider == slider_airlight) {
        label_airlight_value->setText(text);
        sky_K = value;
    } else if (slider == slider_constract) {
        label_constract_value->setText(text);
        clahe_S = value;
    } else if (slider == slider_clip) {
        label_clip_value->setText(text);
        clahe_L = value;
    } else if (slider == slider_sample) {
        label_sample_value->setText(text);
        sample = value;
    }
}

void DarkWidget::SlotSwitchGuide(bool is_open) {
    slider_guide->setEnabled(is_open);
    is_guide = is_open;
}

void DarkWidget::SlotSwitchSky(bool is_open) {
    slider_airlight->setEnabled(is_open);
    is_sky = is_open;
}

void DarkWidget::SlotSwitchSample(bool is_open) {
    slider_sample->setEnabled(is_open);
    is_sample = is_open;
}

void DarkWidget::SlotSwitchFast(bool is_open) {
    is_fast = is_open;
}
