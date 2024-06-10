#include "screenflashlayer.h"


ScreenFlashLayer::ScreenFlashLayer(QGraphicsScene *scene,const std::string &phoneNumber,QWidget *parent)
    : QGraphicsView(scene, parent) {
    emitter = new PatternEmitter(phoneNumber, parent);
    recLayer = new QGraphicsRectItem(0,0, scene->itemsBoundingRect().width(), scene->itemsBoundingRect().height());
    scene->addItem(recLayer);
    recLayer->setPen(Qt::NoPen);
    connect(emitter,&PatternEmitter::emitDash,this,&ScreenFlashLayer::dashLayerColorChange);
    connect(emitter,&PatternEmitter::emitDot,this,&ScreenFlashLayer::dotLayerColorChange);
    connect(emitter,&PatternEmitter::emitSpace,this,&ScreenFlashLayer::spaceLayerColorChange);
}


void ScreenFlashLayer::dashLayerColorChange(){
    QColor dashColor = QColor();
    dashColor.setRed(217);
    dashColor.setGreen(217);
    dashColor.setBlue(217);
    dashColor.setAlpha(30);
    recLayer->setBrush(QBrush(dashColor));
}

void ScreenFlashLayer::dotLayerColorChange(){
    QColor dashColor = QColor();
    dashColor.setRed(225);
    dashColor.setGreen(225);
    dashColor.setBlue(225);
    dashColor.setAlpha(30);
    recLayer->setBrush(QBrush(dashColor));
}

void ScreenFlashLayer::spaceLayerColorChange(){
    QColor dashColor = QColor();
    dashColor.setRed(255);
    dashColor.setGreen(255);
    dashColor.setBlue(255);
    dashColor.setAlpha(30);
    recLayer->setBrush(QBrush(dashColor));
}

void ScreenFlashLayer::startFlasing(){
    emitter->start();
}

void ScreenFlashLayer::updateSize(qint64 x, qint64 y,qint64 width,qint64 height) {
    recLayer->setRect(x,y,width,height);
    recLayer->update(x,y,width,height);
}
