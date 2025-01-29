#ifndef CUSTOMGRAPHICSTEXTITEM_H
#define CUSTOMGRAPHICSTEXTITEM_H

#include <mizushirushihandora.h>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QFont>
#include <QPen>
struct LgMizu;


class CustomGraphicsTextItem : public QLabel {
    Q_OBJECT

public:
    explicit CustomGraphicsTextItem(const QString &text, LgMizu *config, QWidget *parent = nullptr);

    // Setters for properties
    void setOutlineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setOutlineThickness(int thickness);
    void setRotationAngle(qreal angle);

protected:
    // Overridden paint event
    void paintEvent(QPaintEvent *event) override;

private:
    QColor outlineColor;       // Color of the outline
    QColor textColor;          // Color of the text
    int outlineThickness;      // Thickness of the outline
    qreal m_rotationAngle;     // Rotation angle in degrees
    LgMizu *config;            // Configuration pointer
};

#endif // CUSTOMGRAPHICSTEXTITEM_H
