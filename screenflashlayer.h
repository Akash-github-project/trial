#ifndef SCREENFLASHLAYER_H
#define SCREENFLASHLAYER_H

#include <patternemitter.h>
#include <QObject>
#include <QRgba64>
#include <QMouseEvent>
#include <mizushirushihandora.h>

#define FLASH_TIMER flash_seekbar

class ScreenFlashLayer : public QWidget
{
    Q_OBJECT

signals:

    void doubleClickedScreen();
public:
    ScreenFlashLayer(QWidget *scene,const std::string &phoneNumber,FlashMizu * config,QWidget *parent = nullptr);
    void updateSize(qint64 x, qint64 y,qint64 width,qint64 height);
    void startFlasing();
private:
    QWidget *recLayer;
    PatternEmitter *emitter;
    FlashMizu * config;
private:
    void dashLayerColorChange();
    void dotLayerColorChange();
    void spaceLayerColorChange();

};

#endif // SCREENFLASHLAYER_H
