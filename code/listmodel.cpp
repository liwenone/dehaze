#include "listmodel.h"

ListModel::ListModel(QList<Item> list_items0) {
    list_items = list_items0;
}

ListModel::~ListModel() {

}

int ListModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return list_items.size();
}

int ListModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return list_items.size();
}

QVariant ListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() > list_items.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return list_items.at(index.row()).GetText();
    }

    if (role == Qt::DecorationRole) {
        return list_items.at(index.row()).GetImage();;
    }

    return QVariant();
}
