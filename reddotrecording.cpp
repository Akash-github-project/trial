#include "reddotrecording.h"

//RedDotRecording::RedDotRecording() {}

RedDotRecording::RedDotRecording(QGraphicsScene *scene,const std::string phoneNumber,DFlashMizu * config, QWidget *parent)
    : QGraphicsView(scene, parent) {

    emitter = new PatternEmitter(phoneNumber, parent);


    connect(emitter,&PatternEmitter::emitDash,this,&RedDotRecording::dashLayerColorChange);
    connect(emitter,&PatternEmitter::emitDot,this,&RedDotRecording::dotLayerColorChange);
    connect(emitter,&PatternEmitter::emitSpace,this,&RedDotRecording::spaceLayerColorChange);

    // BLINK_TIMER = new QTimer(parent);
    // BLINK_TIMER->setInterval(1000);
    redDot = new QGraphicsEllipseItem(0,0, 20, 20);
    redDot->setBrush(QBrush(QColor(config->color)));

    QGraphicsTextItem *recText = new QGraphicsTextItem("REC");
    recText->setPos((redDot->boundingRect().width() - recText->boundingRect().width()) / 2, redDot->boundingRect().height());
    recText->setDefaultTextColor(QColor(config->color));

    recGroup = new QGraphicsItemGroup();
    recGroup->addToGroup(redDot);
    recGroup->addToGroup(recText);
    scene->addItem(recGroup);
    this->config = config;
    startFlasing();
}


void RedDotRecording::updatePosition(qint64 x, qint64 y) {
    // Position the group at the bottom of the view
    recGroup->setPos(x, y);
}

void RedDotRecording::dotLayerColorChange(){
    QColor dashColor = QColor(config->color);
    dashColor.setAlpha(config->transparency[0]);
    redDot->setBrush(QBrush(dashColor));
}

void RedDotRecording::dashLayerColorChange(){
    QColor dashColor = QColor(config->color);
    dashColor.setAlpha(config->transparency[1]);
    redDot->setBrush(QBrush(dashColor));
}

void RedDotRecording::spaceLayerColorChange(){
    QColor dashColor = QColor(config->color);
    dashColor.setAlpha(config->transparency[2]);
    redDot->setBrush(QBrush(dashColor));
}

void RedDotRecording::startFlasing(){
    emitter->start(config->chn_duration);
}
