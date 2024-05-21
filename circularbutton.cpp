#include "circularbutton.h"

#include <QPainter>

CircularButton::CircularButton(QWidget *parent) : QPushButton(parent)
{

}

void CircularButton::paintEvent(QPaintEvent *)
{
    //Do not paint base implementation -> no styles are applied

    QColor background = isDown() ? QColor("grey") : QColor("lightgrey");
    int diameter = qMin(height(), width());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.translate(width() / 2, height() / 2);

    painter.setPen(QPen(QColor("black"), 2));
    painter.setBrush(QBrush(background));
    painter.drawEllipse(QRect(-diameter / 2, -diameter / 2, diameter, diameter));

}

void CircularButton::resizeEvent(QResizeEvent *e)
{
    QPushButton::resizeEvent(e);
    int diameter = qMin(height(), width())+4 ;
    int xOff =(width() -diameter ) / 2;
    int yOff =(height() - diameter) / 2;
    setMask(QRegion(xOff,yOff, diameter, diameter,QRegion::Ellipse));
}
