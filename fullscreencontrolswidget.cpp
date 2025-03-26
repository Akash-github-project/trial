#include "fullscreencontrolswidget.h"

#include <QGraphicsOpacityEffect>
#include <QMouseEvent>
#include <QPropertyAnimation>
FullScreenControlsWidget::FullScreenControlsWidget(QWidget *parent,QWidget* controlWidget)
    : QWidget(parent), inactivityTimer(new QTimer(this)) {
    // Configure the timer
    setMouseTracking(true);
    this->controlWidget = controlWidget;
    inactivityTimer->setInterval(3000); // 3 seconds
    inactivityTimer->setSingleShot(true);
    connect(inactivityTimer, &QTimer::timeout, this, [controlWidget,this](){
        if(!controlWidget->underMouse()){
            this->animateVisibility(controlWidget,false);
        }
        //this->controlWidget->hide();
    });

    // this->setStyleSheet("background-color:blue;");
    // Initially hidden
    //animateVisibility(this->controlWidget,false);
    this->controlWidget->hide();
}

void FullScreenControlsWidget::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    event->accept();
    //if(!isVisible()){
    showAndResetTimer();
    //}
}

void FullScreenControlsWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
    emit exitFullScreen();
}

void FullScreenControlsWidget::showAndResetTimer() {
    this->controlWidget->show(); // Ensure the widget is visible
    //if(!this->controlWidget->isVisible()){
    if(!isFullyShown){
        animateVisibility(this->controlWidget,true);
    }
    //}
    inactivityTimer->start(); // Restart the timer
}


void FullScreenControlsWidget::animateVisibility(QWidget *widget , bool visible) {
    // Create an opacity effect if the widget doesn't already have one
    int duration  = visible?10 :500;
    QGraphicsOpacityEffect *opacityEffect = dynamic_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(opacityEffect);
    }

    // Create the animation
    QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(duration);
    animation->setStartValue(visible ? 0.0 : 1.0);
    animation->setEndValue(visible ? 1.0 : 0.0);

    // Show the widget if it's being faded in
    widget->show();

    // Connect the animation's finished signal to hide the widget if it's being faded out
    QObject::connect(animation, &QPropertyAnimation::finished, [widget, visible,this]() {
        QPoint point = QCursor::pos();
        if (!visible && !this->controlWidget->rect().contains(point,true)) {
            widget->hide();
            this->isFullyShown = false;
        }
        else {
            this->isFullyShown = true;
        }
    });

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
