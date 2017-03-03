#include "itemdelegate.h"

#include <QDebug>

ItemDelegate::ItemDelegate(QWidget *p = 0)
{
    parent = p;
    width = parent->width();
    height = 60;
}

ItemDelegate::~ItemDelegate() {

}

void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
    QStyleOptionViewItem myOption = option;

    int y  = index.row() * height;

    if (myOption.state & QStyle::State_Selected) {
        painter->fillRect(0, y, width, height, QColor("#EEE8AA"));
    } else if (myOption.state & QStyle::State_MouseOver) {
        painter->fillRect(0, y, width, height, QColor("#FAFAD2"));
    }

    painter->setPen(QColor("#fff"));
    painter->drawLine(0, y, width, y);
    painter->drawLine(0, y + height, width, y + height);
    painter->setPen(QColor("#444"));

    QImage image = qvariant_cast<QImage>(index.data(Qt::DecorationRole));
    painter->drawImage(10 ,y + 10, image.scaled(40, 40));

    QString texts = qvariant_cast<QString>(index.data(Qt::DisplayRole));
    QStringList list_str = texts.split(" ");

    painter->drawText(60, y + 25, list_str[0]);
    painter->drawText(60, y + 45, "本阶段耗时： " + list_str[1] + "ms");
}

bool ItemDelegate::editorEvent(QEvent* event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index) {
    Q_UNUSED(model);
    Q_UNUSED(option);

    if (event->type() == QEvent::MouseButtonPress) {
        if (index.isValid()) {
            QImage image = qvariant_cast<QImage>(index.data(Qt::DecorationRole));
            emit SignalShowImage(image);
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
    Q_UNUSED(option);
    Q_UNUSED(index);

    return QSize(width, height);
}

