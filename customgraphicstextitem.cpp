#include "customgraphicstextitem.h"
#include <QPainter>
#include <QPainterPath>

CustomGraphicsTextItem::CustomGraphicsTextItem(const QString &text,LgMizu * config,QGraphicsItem *parent)
    : QGraphicsTextItem(text, parent),
    outlineColor(Qt::black),
    textColor(Qt::white),
    outlineThickness(10) {
    this->config = config;
}

void CustomGraphicsTextItem::setOutlineColor(const QColor &color) {
    outlineColor = color;
    update();  // Trigger repaint
}

void CustomGraphicsTextItem::setTextColor(const QColor &color) {
    textColor = color;
    update();  // Trigger repaint
}

void CustomGraphicsTextItem::setOutlineThickness(int thickness) {
    outlineThickness = thickness;
    update();  // Trigger repaint
}

void CustomGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Enable anti-aliasing for smooth edges
    painter->setRenderHint(QPainter::Antialiasing);

    // Set up font
    QFont font = QFont("Arial",config->font_size * 3, QFont::Bold);
    painter->setFont(font);


    // Create a shadow color (a semi-transparent black)
    QColor shadowColor = QColor(0, 0, 0, 160);  // Adjust alpha for transparency

    // Draw shadow text slightly offset
    painter->setPen(shadowColor);
    painter->setBrush(shadowColor);
    painter->drawText(2, 2, toPlainText());  // Offset shadow by (2, 2)

    // Draw the main text
    painter->setPen(QColor(config->color));  // Set text color
    painter->setBrush(QColor(config->color));  // Fill the text
    painter->drawText(0, 0, toPlainText());  // Draw main text at the original position
}
