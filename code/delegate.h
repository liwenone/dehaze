#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QPixmap>
#include <QColor>
#include <QEvent>
#include <QAbstractItemModel>
#include <QWidget>

#include "item.h"

class ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ItemDelegate(QWidget* p, int type0);
    ~ItemDelegate();

protected:
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    bool editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index);
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

signals:
    void chatWithUser(QStringList);
    void chatWithGroup(QStringList);

private:
    QWidget* parent;
    int width;
    int heigth;
    int type;
};
#endif // DELEGATE_H
