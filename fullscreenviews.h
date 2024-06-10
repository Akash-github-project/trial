#ifndef FULLSCREENVIEWS_H
#define FULLSCREENVIEWS_H

#include <QObject>
#include <QtWidgets>
#include <VideoProgressBarController.h>
//#include "ui_mainwindow.h"
#include <QUiLoader>
#include <FullScreenControlHoverHandler.h>

// class FullScreenViews : public QObject
// {
//     Q_OBJECT
// public:
//     explicit FullScreenViews(QObject *parent ,Ui::MainWindow *mainWindow,QMainWindow *window);
//     VideoProgressBarController *fsSeekbarController = nullptr;
//     QPushButton *fsPlayPauseButton = nullptr;
//     QPushButton *fsStopButton = nullptr;
//     QPushButton *fsTenSecForward = nullptr;
//     QPushButton *fsTenSecBackward = nullptr;
//     QPushButton *fsNormalButton = nullptr;
//     QPushButton *fsSpeed1x = nullptr;
//     QPushButton *fsSpeed1p2x = nullptr;
//     QPushButton *fsSpeed1p5x = nullptr;
//     QPushButton *fsSpeed2x = nullptr;
//     QSlider *fsSeekbar = nullptr;
//     QSlider *fsSeekbarVolume = nullptr;
//     QLabel *fsCurrentTime = nullptr;
//     QLabel *fsTotalTime = nullptr;
//     QMetaObject::Connection fsSeekbarConnection;
//     QMetaObject::Connection fsSeekbarForwardBackwardConnection;
//     QWidget *controls;

//     void goBackToNormalScreen(Ui::MainWindow *ui, MainWindow *window);
// signals:

// private:
//     bool isFullScreen;
//     void loadFullScreenControls(Ui::MainWindow *mainWindow,MainWindow *window);
// };

#endif // FULLSCREENVIEWS_H
