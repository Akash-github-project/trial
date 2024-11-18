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
#define BLINK_TIMER djLighting

class RedDotRecording : public QGraphicsView
{
    Q_OBJECT
public:
    RedDotRecording(QGraphicsScene *scene,const std::string phoneNumber,DFlashMizu * config,QWidget *parent = nullptr);
    void updatePosition(qint64 x, qint64 y);
    void startFlasing();
private:
    DFlashMizu * config;
    QTimer *BLINK_TIMER;
    QGraphicsItemGroup *recGroup;
    QGraphicsEllipseItem *redDot;
    PatternEmitter *emitter;
    bool isDeepRed = false;
    void dotLayerColorChange();
    void spaceLayerColorChange();
    void dashLayerColorChange();
};

#endif // REDDOTRECORDING_H
