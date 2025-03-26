#include "reddotwidget.h"

RedDotWidget::RedDotWidget(QWidget *parent, const QColor &color)
    : QWidget(parent), dotColor(color) {
    setFixedSize(20, 20); // Set size of the dot
    this->parent = parent;
    this->setStyleSheet("background-color:yellow;");
}

void RedDotWidget::setColor(QColor color)
{
    dotColor = color;
    update();
}

void RedDotWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(dotColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, width(), height()); // Draw the circle
}
