#include "customseekbar.h"

CustomSeekbar::CustomSeekbar(QWidget *parent) : QSlider(parent) { }

void CustomSeekbar::onSliderClick(QPointF point,QRectF parentRect){
    QPointF localPoint = mapFromParent(point);
    QPoint containingPoint = localPoint.toPoint();
    QRect localRect = parentRect.toRect();

    int finalRectx = localRect.topLeft().x() + this->geometry().x();
    int finalRecty = localRect.topLeft().y() + this->geometry().y();
    QRect seekbarBox(finalRectx,finalRecty,width(),height() + 10);
    qDebug()<<"seekbar box"<<seekbarBox;
    qDebug()<<"containing point"<<point.toPoint();
    qDebug()<<"seekbar width"<<width();
    int maxValue = maximum();
    int minValue = minimum();

    // Calculate the maximum allowed value as 99% of the maximum value
    int maxAllowedValue = minValue + (maxValue - minValue) * 0.99;

    int newValue;
    if (orientation() == Qt::Vertical)
    {
        newValue = minValue + ((maxValue - minValue) * (height() - containingPoint.y())) / height();
    }
    else
    {
        //testing
        //newValue = minValue + ((maxValue - minValue) * (containingPoint.x() - 4)) / width();
        newValue = minValue + ((maxValue - minValue) * (containingPoint.x()) - 4) / width();
        qDebug()<<minValue << " + " <<"(" <<maxValue <<" - " <<minValue <<") * " << containingPoint.x() - 4 << " / " << width();
        qDebug()<<"newValue "<<newValue;
        qDebug()<<"maxAllowedValue" << maxAllowedValue;
    }
    // Restrict the value to the maximum allowed value
    if (newValue > maxAllowedValue)
    {
        newValue = maxAllowedValue;
    }
    setValue(newValue);
}
