#ifndef REDDOTRECORDING_H
#define REDDOTRECORDING_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <patternemitter.h>
#include <mizushirushihandora.h>
#include "recwidget.h"
#define BLINK_TIMER djLighting

class RedDotRecording : public QWidget
{
    Q_OBJECT
public:
    RedDotRecording(QWidget *scene,const std::string phoneNumber,DFlashMizu * config,QWidget *parent = nullptr);
    void updatePosition(qint64 x, qint64 y);
    void startFlasing();
private:
    DFlashMizu * config;
    QTimer *BLINK_TIMER;
    // QGraphicsItemGroup *recGroup;
    // QGraphicsEllipseItem *redDot;
    PatternEmitter *emitter;
    RecWidget *recWidget;
    bool isDeepRed = false;
    void dotLayerColorChange();
    void spaceLayerColorChange();
    void dashLayerColorChange();
    void dollarLayerColorChange();
};

#endif // REDDOTRECORDING_H
