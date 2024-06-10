#ifndef SCREENFLASHLAYER_H
#define SCREENFLASHLAYER_H

#include <patternemitter.h>
#include <QGraphicsView>
#include <QObject>
#include <QGraphicsRectItem>
#include <QRgba64>
#define FLASH_TIMER flash_seekbar

class ScreenFlashLayer : public QGraphicsView
{
    Q_OBJECT

public:
    ScreenFlashLayer(QGraphicsScene *scene,const std::string &phoneNumber, QWidget *parent = nullptr);
    void updateSize(qint64 x, qint64 y,qint64 width,qint64 height);
    void startFlasing();
private:
    QGraphicsRectItem *recLayer;
    PatternEmitter *emitter;
private:
    void dashLayerColorChange();
    void dotLayerColorChange();
    void spaceLayerColorChange();
};

#endif // SCREENFLASHLAYER_H
