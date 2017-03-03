#ifndef ITEM_H
#define ITEM_H

#include <QImage>
#include <QString>

using namespace std;

class Item
{
public:
    Item(QImage image0, QString name0, int time0);

    QImage GetImage() const;
    QString GetText() const;

private:
    QImage image;
    QString text;
};

#endif // ITEM_H
