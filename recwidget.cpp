#include "recwidget.h"

RecWidget::RecWidget(QWidget *parent, const QColor &color)
    : QWidget(parent)
{
    //this->setGeometry(parent->width() - 40,10,20,40);
    // Create the red dot
    redDot = new RedDotWidget(this, QColor(255,0,0,0));

    // Create the "REC" text
    recText = new QLabel("REC", this);
    // recText->hide();
    recText->setAlignment(Qt::AlignCenter);
    recText->setStyleSheet(QString("color: %1; font-weight: bold;").arg(color.name()));

    // Add to a vertical layout
    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(redDot);
    layout->addWidget(recText);
    setLayout(layout);
}



void RecWidget::setColor(QColor color)
{
    recText->setStyleSheet(QString("color: %1; font-weight: bold;").arg(color.name()));
    redDot->setColor(color);
    // if(recText->isHidden()){
    //     recText->show();
    // }
    //this->move(40,10);
}
