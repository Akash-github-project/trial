#ifndef CUSTOMGRAPHICSTEXTITEM_H
#define CUSTOMGRAPHICSTEXTITEM_H

#include <mizushirushihandora.h>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QFont>
#include <QPen>
struct LgMizu;

class CustomGraphicsTextItem : public QGraphicsTextItem {
public:
    CustomGraphicsTextItem(const QString &text,LgMizu* config ,QGraphicsItem *parent = nullptr);
    void setOutlineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setOutlineThickness(int thickness);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QColor outlineColor;
    QColor textColor;
    int outlineThickness;
    LgMizu * config;
};


#endif // CUSTOMGRAPHICSTEXTITEM_H
