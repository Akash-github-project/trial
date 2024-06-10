#include "reddotrecording.h"

//RedDotRecording::RedDotRecording() {}

RedDotRecording::RedDotRecording(QGraphicsScene *scene,const std::string phoneNumber, QWidget *parent)
    : QGraphicsView(scene, parent) {

    emitter = new PatternEmitter(phoneNumber, parent);


    connect(emitter,&PatternEmitter::emitDash,this,&RedDotRecording::dashLayerColorChange);
    connect(emitter,&PatternEmitter::emitDot,this,&RedDotRecording::dotLayerColorChange);
    connect(emitter,&PatternEmitter::emitSpace,this,&RedDotRecording::spaceLayerColorChange);

    // BLINK_TIMER = new QTimer(parent);
    // BLINK_TIMER->setInterval(1000);
    redDot = new QGraphicsEllipseItem(0,0, 20, 20);
    redDot->setBrush(QBrush(Qt::red));

    QGraphicsTextItem *recText = new QGraphicsTextItem("REC");
    recText->setPos((redDot->boundingRect().width() - recText->boundingRect().width()) / 2, redDot->boundingRect().height());
    recText->setDefaultTextColor(Qt::red);

    recGroup = new QGraphicsItemGroup();
    recGroup->addToGroup(redDot);
    recGroup->addToGroup(recText);
    scene->addItem(recGroup);

    // BLINK_TIMER->start();
    // connect(BLINK_TIMER,&QTimer::timeout,[&]{
    //     if(this->isDeepRed){
    //         redDot->setBrush(QBrush(Qt::red));
    //     } else {
    //         redDot->setBrush(QBrush(Qt::darkRed));
    //     }
    //     this->isDeepRed = !this->isDeepRed;
    // });
    startFlasing();
}


void RedDotRecording::updatePosition(qint64 x, qint64 y) {
    // Position the group at the bottom of the view
    recGroup->setPos(x, y);
}


void RedDotRecording::dashLayerColorChange(){
    QColor dashColor = QColor(Qt::red);
    dashColor.setAlpha(30);
    redDot->setBrush(QBrush(dashColor));
}

void RedDotRecording::dotLayerColorChange(){
    QColor dashColor = QColor(Qt::red);
    dashColor.setAlpha(60);
    redDot->setBrush(QBrush(dashColor));
}

void RedDotRecording::spaceLayerColorChange(){
    QColor dashColor = QColor(Qt::red);
    dashColor.setAlpha(90);
    redDot->setBrush(QBrush(dashColor));
}

void RedDotRecording::startFlasing(){
    emitter->start();
    //recGroup->setPos(0,recGroup->boundingRect().height() - 40);
}
