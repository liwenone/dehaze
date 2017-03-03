#include "item.h"

Item::Item(QImage image0, QString name0, int time0)
{
    image = image0;

    text = name0 + " " + QString::number(time0);
}

QImage Item::GetImage() const {
    return image;
}

QString Item::GetText() const{
    return text;
}
