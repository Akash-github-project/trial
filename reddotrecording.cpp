#include "reddotrecording.h"


//RedDotRecording::RedDotRecording() {}

RedDotRecording::RedDotRecording(QWidget *scene,const std::string phoneNumber,DFlashMizu * config, QWidget *parent)
    : QWidget(parent) {

    emitter = new PatternEmitter(phoneNumber, parent);


    connect(emitter,&PatternEmitter::emitDash,this,&RedDotRecording::dashLayerColorChange);
    connect(emitter,&PatternEmitter::emitDot,this,&RedDotRecording::dotLayerColorChange);
    connect(emitter,&PatternEmitter::emitSpace,this,&RedDotRecording::spaceLayerColorChange);

    // redDot = new QGraphicsEllipseItem(0,0, 20, 20);
    // redDot->setBrush(QBrush(QColor(config->color)));

    // QGraphicsTextItem *recText = new QGraphicsTextItem("REC");
    // recText->setPos((redDot->boundingRect().width() - recText->boundingRect().width()) / 2, redDot->boundingRect().height());
    // recText->setDefaultTextColor(QColor(config->color));

    // recGroup = new QGraphicsItemGroup();
    // recGroup->addToGroup(redDot);
    // recGroup->addToGroup(recText);
    // scene->addItem(recGroup);


    // Instantiate the RecWidget
    recWidget = new RecWidget(scene, QColor(config->color));

    // Add it to the parent container (e.g., main window or another widget)
    //recWidget->setParent(parentWidget);
    recWidget->move(100, 100); // Set position if needed
    recWidget->show();


    this->config = config;
    startFlasing();
}


void RedDotRecording::updatePosition(qint64 x, qint64 y) {
    // Position the group at the bottom of the view
    if(recWidget != nullptr){
        recWidget->move(x,y);
    }
}

void RedDotRecording::dotLayerColorChange(){
    QColor dashColor = QColor(config->color);
    dashColor.setAlpha(config->transparency[0]);
    //redDot->setBrush(QBrush(dashColor));
    recWidget->setColor(dashColor);
}

void RedDotRecording::dashLayerColorChange(){
    QColor dashColor = QColor(config->color);
    dashColor.setAlpha(config->transparency[1]);
    //redDot->setBrush(QBrush(dashColor));
    recWidget->setColor(dashColor);
}

void RedDotRecording::spaceLayerColorChange(){
    QColor dashColor = QColor(config->color);
    dashColor.setAlpha(config->transparency[2]);
   // redDot->setBrush(QBrush(dashColor));
    recWidget->setColor(dashColor);
}

void RedDotRecording::startFlasing(){
    emitter->start(config->chn_duration);
}
