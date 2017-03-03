#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractItemModel>
#include <QVariant>
#include <QModelIndex>
#include <QPixmap>
#include <QImage>

#include "item.h"

class ListModel : public QAbstractListModel
{
public:
    ListModel(QList<Item> list_items0);
    ~ListModel();

protected:
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex & index, int role) const;

private:
    QList<Item> list_items;
};

#endif // LISTMODEL_H
