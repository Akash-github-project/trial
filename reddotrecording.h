#ifndef REDDOTRECORDING_H
#define REDDOTRECORDING_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <patternemitter.h>
#define BLINK_TIMER djLighting

class RedDotRecording : public QGraphicsView
{
    Q_OBJECT
public:
    RedDotRecording(QGraphicsScene *scene,const std::string phoneNumber ,QWidget *parent = nullptr);
    void updatePosition(qint64 x, qint64 y);
    void startFlasing();
private:
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
