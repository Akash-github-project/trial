#ifndef SCREENFLASHLAYER_H
#define SCREENFLASHLAYER_H

#include <patternemitter.h>
#include <QGraphicsView>
#include <QObject>
#include <QGraphicsRectItem>
#include <QRgba64>
#include <QMouseEvent>

#define FLASH_TIMER flash_seekbar

class ScreenFlashLayer : public QGraphicsView
{
    Q_OBJECT

signals:

    void doubleClickedScreen();
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
