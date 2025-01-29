#include "customgesturewidget.h"

#include <QMouseEvent>

CustomGestureWidget::CustomGestureWidget(QWidget *parent)
    : QWidget{parent}
{}

void CustomGestureWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    emit makeFullScreen();
}
