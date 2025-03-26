#include "screenflashlayer.h"


ScreenFlashLayer::ScreenFlashLayer(QWidget *scene,const std::string &phoneNumber,FlashMizu * config,QWidget *parent)
    : QWidget( parent) {
    this->config = config;
    emitter = new PatternEmitter(phoneNumber,this->config->chn_duration,parent);
    recLayer = new QWidget(scene);
    //scene->addItem(recLayer);
    //recLayer->setPen(Qt::NoPen);
    connect(emitter,&PatternEmitter::emitDash,this,&ScreenFlashLayer::dashLayerColorChange);
    connect(emitter,&PatternEmitter::emitDot,this,&ScreenFlashLayer::dotLayerColorChange);
    connect(emitter,&PatternEmitter::emitSpace,this,&ScreenFlashLayer::spaceLayerColorChange);
}

void ScreenFlashLayer::dotLayerColorChange(){
    QColor dashColor = QColor(config->color);
    // dashColor.setRed(225);
    // dashColor.setGreen(225);
    // dashColor.setBlue(225);
    dashColor.setAlpha(config->transparency[0]);
    recLayer->setStyleSheet(QString("background-color: %1;").arg(dashColor.name(QColor::HexArgb)));
    //recLayer->setBrush(QBrush(dashColor));
}

void ScreenFlashLayer::dashLayerColorChange(){
    QColor dashColor = QColor(config->color);
    // dashColor.setRed(217);
    // dashColor.setGreen(217);
    // dashColor.setBlue(217);
    dashColor.setAlpha(config->transparency[1]);
    recLayer->setStyleSheet(QString("background-color: %1;").arg(dashColor.name(QColor::HexArgb)));
}

void ScreenFlashLayer::spaceLayerColorChange(){
    QColor dashColor = QColor(config->color);
    // dashColor.setRed(255);
    // dashColor.setGreen(255);
    // dashColor.setBlue(255);
    dashColor.setAlpha(config->transparency[2]);
    recLayer->setStyleSheet(QString("background-color: %1;").arg(dashColor.name(QColor::HexArgb)));
}

void ScreenFlashLayer::startFlasing(){
    emitter->start(config->chn_duration);
}

void ScreenFlashLayer::updateSize(qint64 x, qint64 y,qint64 width,qint64 height) {
    recLayer->setGeometry(x,y,width,height);
    //recLayer->setRect(x,y,width,height);
    //recLayer->update(x,y,width,height);
}
