#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QColor>
#include <QEvent>
#include <QAbstractItemModel>
#include <QWidget>
#include <QImage>

#include "item.h"

class ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

signals:
    void SignalShowImage(QImage image);

public:
    ItemDelegate(QWidget* p);
    ~ItemDelegate();

protected:
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    bool editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index);
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

private:
    QWidget* parent;
    int width;
    int height;
};

#endif // ITEMDELEGATE_H
