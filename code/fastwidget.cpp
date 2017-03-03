#include "fastwidget.h"
#include "stylesheet.h"

#include <QDebug>

FastWidget::FastWidget(QWidget *parent) : QWidget(parent)
{
    ave_filter_R = 7;
    eps = 0.5;
    clahe_L = 2;
    clahe_S = 2;

    fast = NULL;


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

    // 主布局
    QVBoxLayout* layout_main = new QVBoxLayout();
    layout_main->setMargin(0);
    layout_main->setSpacing(0);
    layout_main->addWidget(widget_title);
    layout_main->addWidget(list_view);
    layout_main->addWidget(widget_control);

    this->setLayout(layout_main);
}

QWidget* FastWidget::CreateTitleWidget() {
    QWidget* widget = new QWidget();
    widget->setStyleSheet(WIDGET_LEFT_TOP);
    widget->setFixedHeight(90);

    QLabel* label_title = new QLabel("快速去雾");
    label_title->setStyleSheet(LABEL_TITLE);
    label_title->setFixedHeight(40);

    QPushButton* btn_open = new QPushButton("打开");
    QPushButton* btn_dehaze = new QPushButton("去雾");
    btn_open->setStyleSheet(BTN_TITLE);
    btn_dehaze->setStyleSheet(BTN_TITLE);

    connect(btn_open, SIGNAL(clicked(bool)), this, SLOT(SlotOpenFile()));
    connect(btn_dehaze, SIGNAL(clicked(bool)), this, SLOT(SlotDehaze()));

    QHBoxLayout* layout_btn = new QHBoxLayout();
    layout_btn->setMargin(5);
    layout_btn->addWidget(btn_open);
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

QWidget* FastWidget::CreateControlWidget() {
    QWidget* widget_control = new QWidget();


    // 控制因素标签
    QLabel* label_ave = new QLabel("均值滤波半径：");
    QLabel* label_eps = new QLabel("大气光因子：");
    QLabel* label_clip = new QLabel("CLAHE剪裁限幅：");
    QLabel* label_constract = new QLabel("CLAHE块大小：");
    // 将控制因素标签添加到布局
    QVBoxLayout* layout_left = new QVBoxLayout();
    layout_left->addWidget(label_ave);
    layout_left->addWidget(label_eps);
    layout_left->addWidget(label_clip);
    layout_left->addWidget(label_constract);

    // 控制值
    label_ave_value = new QLabel();
    label_eps_value = new QLabel();
    label_clip_value = new QLabel();
    label_constract_value = new QLabel();
    label_ave_value->setFixedWidth(23);
    // 添加到布局
    QVBoxLayout* layout_right = new QVBoxLayout();
    layout_right->addWidget(label_ave_value);
    layout_right->addWidget(label_eps_value);
    layout_right->addWidget(label_clip_value);
    layout_right->addWidget(label_constract_value);

    // 控制滑块
    slider_ave = new QSlider(Qt::Horizontal);
    slider_eps = new QSlider(Qt::Horizontal);
    slider_clip = new QSlider(Qt::Horizontal);
    slider_constract = new QSlider(Qt::Horizontal);

    slider_ave->setStyleSheet(SLIDER);
    slider_eps->setStyleSheet(SLIDER);
    slider_constract->setStyleSheet(SLIDER);
    slider_clip->setStyleSheet(SLIDER);

    connect(slider_ave, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));
    connect(slider_eps, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));
    connect(slider_clip, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));
    connect(slider_constract, SIGNAL(valueChanged(int)), SLOT(SlotSetControlLabelText(int)));

    // 初始化滑块的值
    slider_ave->setValue(ave_filter_R);
    slider_ave->setMinimum(1);
    slider_ave->setMaximum(100);

    slider_eps->setValue(eps * 10);
    slider_eps->setMinimum(0);
    slider_eps->setMaximum(10);

    slider_clip->setValue(clahe_L);
    slider_clip->setMinimum(1);
    slider_clip->setMaximum(100);

    slider_constract->setValue(clahe_S);
    slider_constract->setMinimum(1);
    slider_constract->setMaximum(100);

    // 将控制滑块添加到布局
    QVBoxLayout* layout_center = new QVBoxLayout();
    layout_center->addWidget(slider_ave);
    layout_center->addWidget(slider_eps);
    layout_center->addWidget(slider_clip);
    layout_center->addWidget(slider_constract);

    QHBoxLayout* layout_main = new QHBoxLayout();
    layout_main->addLayout(layout_left);
    layout_main->addLayout(layout_center);
    layout_main->addLayout(layout_right);

    widget_control->setLayout(layout_main);

    return widget_control;
}

void FastWidget::SlotOpenFile() {
    QString file_name = QFileDialog::getOpenFileName(this, "选择图片", "./Pictures", "image files(*.png *.jpeg *.jpg)");
    if (file_name.isEmpty() || file_name.isNull()) {
        qDebug() << "打开失败";
        return;
    }

    // 清理list
    list_items.clear();
    model = new ListModel(list_items);
    list_view->setModel(model);

    if (fast != NULL) {
        delete fast;
    }
    fast = new FastDehaze();
    img_struct = fast->GetInputImStruct(file_name.toStdString());
    this->ClearList();
}

void FastWidget::ClearList() {
    // 清理list
    list_items.clear();
    delete model;

    this->UpdateListView(img_struct, "输入图像");

    QImage img = Tools::ConverMatToImage(img_struct.mat);
    emit SignalShowImage(img);
}

void FastWidget::SlotDehaze() {
    if (fast != NULL) {
        this->ClearList();
        fast->SetArgs(ave_filter_R, eps, clahe_L, clahe_S);

        int i = 0;
        vector<ImgStruct> mats = fast->GetAllMats();
        this->UpdateListView(mats.at(i++), "Min(RGB)图");
        this->UpdateListView(mats.at(i++), "均值滤波图像");
        this->UpdateListView(mats.at(i++), "环境光图像");
        this->UpdateListView(mats.at(i++), "无雾图像");
        this->UpdateListView(mats.at(i++), "色阶增强无雾图像");
    }
}

void FastWidget::UpdateListView(ImgStruct img_struct, QString name) {
    QImage img = Tools::ConverMatToImage(img_struct.mat);

    Item item(img, name, img_struct.time);
    list_items.append(item);

    model = new ListModel(list_items);
    list_view->setModel(model);
}

void FastWidget::SlotShowImage(QImage image) {
     emit SignalShowImage(image);
}

void FastWidget::SlotSetControlLabelText(int value) {
    QString text = QString::number(value);

    QSlider* slider = (QSlider*)this->sender();
    if (slider == slider_ave) {
        label_ave_value->setText(text);
        ave_filter_R = value;
    } else if (slider == slider_eps) {
        eps = value / 10.0f;
        label_eps_value->setText(QString::number(eps));
    } else if (slider == slider_constract) {
        label_constract_value->setText(text);
        clahe_S = value;
    } else if (slider == slider_clip) {
        label_clip_value->setText(text);
        clahe_L = value;
    }
}
