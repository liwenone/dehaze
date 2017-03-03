#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>

// 按钮
const QString BTN_DARK = "QRadioButton::indicator{width:30px; height:30px;border:none;border-radius:15px;}QRadioButton::indicator:checked{background:#FFC0CB;}QRadioButton::indicator:unchecked{background:#F5DEB3;}";
const QString BTN_HISTOGRAM = "QRadioButton::indicator{width:30px; height:30px;border:none;border-radius:15px;}QRadioButton::indicator:checked{background:#FFC0CB;}QRadioButton::indicator:unchecked{background:#F5DEB3;}";
const QString BTN_FAST = "QRadioButton::indicator{width:30px; height:30px;border:none;border-radius:15px;}QRadioButton::indicator:checked{background:#FFC0CB;}QRadioButton::indicator:unchecked{background:#F5DEB3;}";
const QString BTN_VIDEO = "QRadioButton::indicator{width:30px; height:30px;border:none;border-radius:15px;}QRadioButton::indicator:checked{background:#FFC0CB;}QRadioButton::indicator:unchecked{background:#F5DEB3;}";


//
const QString WIDGET_TOP = "QWidget{background: white;}";
const QString WIDGET_LEFT = "QWidget{background: white;font-size:12px;font-family:'微软雅黑'; color:#444;border;}";
const QString WIDGET_RIGHT = "QWidget{background: rgb(56, 67, 71);}";

const QString WIDGET_ITEM = "QWidget{background: #fff;}"
                            "QWidget:hover{background: #eee;}";

const QString LABEL_SHOW_OFF = "QLabel{border:0px solid #F5F5DC}";
const QString LABEL_SHOW_ON = "QLabel{border:3px solid #F5F5DC}";

const QString LIST_VIEW = "QListView{background: #eee;border:none;}";

// 开关
const QString BTN_SWITCH = "QPushButton{border-image: url(':res/switch_check.png'); width:40px; height:25px;border:none;}"
                           "QPushButton:checked{border-image: url(':res/switch_off.png'); width:40px; height:25px;border:none;}";

// 打开、去雾
const QString BTN_TITLE = "QPushButton{background:#EEE8AA; width:40px; height:25px;border:2px solid #F5F5DC;border-radius: 8px;}"
                         "QPushButton:hover{background:#FAFAD2;}";

// 滑块
const QString SLIDER = "QSlider::add-page:horizontal{background:#FAFAD2;border-radius:5px;}QSlider::handle:horizontal{background:#ADFF2F;width:10px;border-radius:5px;}QSlider::groove:horizontal{background:#EEE8AA;border-radius:5px;height:15px;}"
                       "QSlider::add-page:horizontal:disabled{background:#F5F5DC;}QSlider::handle:horizontal:disabled{background:#DCDCDC;}QSlider::groove:horizontal:disabled{background:#F5F5DC;}";

// checkbox
const QString BOX_SWITCH = "QCheckBox::indicator{width:12px;height:12px;border-radius:9px;border:3px solid #F5F5DC;}QCheckBox::indicator:unchecked{background:#EEE8AA;}QCheckBox::indicator:checked{background:#FF6347}";

const QString WIDGET_SWITCH = "QWidget{background:rgb(107,203,243);color:white;}";

const QString WIDGET_LEFT_TOP = "QWidget{background:#DCDCDC;}";


// tittle
const QString BNT_CLOSE = "QPushButton{background:#FFC0CB; width:20px; height:20px;border:none;border-radius:10px;}"
                          "QPushButton:hover{background:#FF9797; width:20px; height:20px;border:none;}";
const QString BNT_MIN = "QPushButton{background:rgb(177, 202, 224); width:20px; height:20px;border:none;border-radius:10px;}"
                          "QPushButton:hover{background:#F3F3FA; width:20px; height:20px;border:none;}";
const QString BNT_MAX = "QPushButton{background:rgb(177, 202, 224); width:20px; height:20px;border:none;border-radius:10px;}"
                          "QPushButton:hover{background:#F3F3FA; width:20px; height:20px;border:none;}";
const QString WIDGET_TITLE = "QWidget{background: rgb(158, 185, 214)}";

const QString LABEL_TITLE = "QLabel{margin-left:5px; font-size:14px; font-weight: bold; color: #888;}";

#endif // STYLESHEET_H
