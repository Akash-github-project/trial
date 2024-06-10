#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videoprogressbarcontroller.h"
#include "encryptionhandler.h"
#include "windoweventhandler.h"
#include "reddotrecording.h"
#include "screenflashlayer.h"

#include <QMainWindow>
#include <QMediaPlayer>
#include <QtMultimedia>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <QVideoWidget>
///
#include <QFile>
#include <QBuffer>
#include <QDir>
#include <QGraphicsVideoItem>
#include <PlaybackRateHandler.h>
#include <SeekbarProgressController.h>
#include <fullscreenviews.h>
#include <windows.h>
#include <QWindow>
//
#define WATERMARK_TEXT random_video
#define RECORDING_RED_DOT random_seekbar
#define RECORDING_FLASH_LAYER random_seekbar_slider

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // void makeButtonRound(QPushButton* button);
    void resizeEvent(QResizeEvent *event);
    void slderClicked(int action);
    void setupFullScreenControls();
    void onPlaybackRateChanged(float playbackRate);
public:
    int fileCount = 0;
    const int timeLimit = 120;
    QGraphicsScene *scene;
     int oldHeight = 0;
     int oldWidth = 0;
    QMediaPlayer *Player;
    // Create a QGraphicsScene
    QGraphicsView *view = nullptr;
    QGraphicsVideoItem *videoItem = nullptr;
    PlaybackRateHandler *playbackRateHandler;
    int getVideoIndexToJump(int timeInSeconds);
public slots:
    void fullScreenChnaged(const QRectF &rect);
    void on_normal_button_pressed();
    void handlePlayPauseButtonState(QMediaPlayer::PlaybackState playbackState);
    void handleVolumeChange(int volume);
    void handleWindowModesTransitions(bool isFullScreen);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 duration);
    void on_actionOpen_triggered();
    void on_pushButton_Play_Pause_clicked();
    void on_pushButton_Stop_clicked();
    void on_pushButton_Volume_clicked();
    void on_horizontalSlider_Volume_valueChanged(int value);
    void on_pushButton_Seek_Backward_clicked();
    void on_pushButton_Seek_Forward_clicked();
    void loadVideo(QMediaPlayer::MediaStatus status);
    QStringList getFileList(const QString& directoryPath);
    void on_horizontalSlider_Duration_sliderMoved();
    void onSliderStop();
    void on_pushButton_1x_clicked();
    void on_pushButton_1p2x_clicked();
    void on_pushButton_1p5x_clicked();
    void on_pushButton_2x_clicked();
    void on_pushButton_full_screen_clicked();

private:
    Ui::MainWindow *ui;
    QVideoWidget *Video = nullptr;
    QGraphicsWidget *GraphicsWidget = nullptr;
    //FullScreenViews  *fullScreenViews = nullptr;
    SeekbarProgressController *seekbarNewController;
    // Create a QGraphicsVideoItem
    qint64 videoTimeArray[5] = {120929,120790,120256,118384,119675};
    qint64 mDuration;
    bool IS_Pause = true;
    bool IS_Muted = false;
    bool isLoading = false;
    bool isChanging = false;
    int currentIndex = 0;
    QString FileName;
    VideoProgressBarController *seekbarController = nullptr;
    QString selectedDirectory;
    QString videoFileChunkPattern = "encrypted*.mp4";
    //QString videoFileChunkPattern = "encrypted_input*.mp4";
    qint64 sliderTime = -1;
    EncryptionHandler *handler = nullptr;
    QGraphicsTextItem *WATERMARK_TEXT = nullptr;
    RedDotRecording *RECORDING_RED_DOT = nullptr;
    ScreenFlashLayer *RECORDING_FLASH_LAYER = nullptr;
    QMetaObject::Connection seekbarConnection ;
    std::string phoneNumber = "828282828228";
    int extraSeekValue = -1;
     bool isFullScreen = false;
     VideoProgressBarController *fsSeekbarController = nullptr;
     QPushButton *fsPlayPauseButton = nullptr;
     QPushButton *fsStopButton = nullptr;
     QPushButton *fsTenSecForward = nullptr;
     QPushButton *fsTenSecBackward = nullptr;
     QPushButton *fsNormalButton = nullptr;
     QPushButton *fsSpeed1x = nullptr;
     QPushButton *fsSpeed1p2x = nullptr;
     QPushButton *fsSpeed1p5x = nullptr;
     QPushButton *fsSpeed2x = nullptr;
     QSlider *fsSeekbar = nullptr;
     QSlider *fsSeekbarVolume = nullptr;
     QLabel *fsCurrentTime = nullptr;
     QLabel *fsTotalTime = nullptr;
     QMetaObject::Connection fsSeekbarConnection;
     QMetaObject::Connection fsSeekbarForwardBackwardConnection;
     QWidget *controls;
     WindowEventHandler *fullScreenEventHandler;
     bool mediaStopped = false;
     QString pauseButtonStyle =
                "QPushButton {"
                "    border: none;"
                "    background-image: url(://pause_button);"
                "    background-repeat: no-repeat;"
                "    background-position: center;"
                "}";

     QString playButtonStyle =
                "QPushButton {"
                "    border: none;"
                "    background-image: url(://play_button);"
                "    background-repeat: no-repeat;"
                "    background-position: center;"
                "}";
     qint64 lastDisplayTime = 0;
private:
     /////////////////
    void updateDuration(qint64 Duration);
    void loadParticalarChunk(int videoIndex, int extraSeek);
    void jumpToPosition(int secondToJump);
    void openParticularChunk(QByteArray byteData);
    void windowStateChange(MainWindow *, int, MainWindow *);
    void handleUserManualFullScreen();
    void seekBackward(long oldTime);
    int getExtraSeek(int timeInSeconds, int indexToJump);
    void setupKeyboardShortcuts();
    void handleUserManualMaximized();
    void handleUserManualUnMaximize();
    void handleArrowKey(int key);
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right ||
                keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
            {
                handleArrowKey(keyEvent->key());
                return true;
            }
        }
        return QMainWindow::eventFilter(watched, event);
    }
    void disableScreenRecording();
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p{this};
        p.fillRect(rect(), {100, 100, 100, 128});
        p.setPen({0, 200, 255});
        p.drawText(rect(), "Loading...", Qt::AlignHCenter | Qt::AlignVCenter);
    }

    void changeEvent(QEvent *event) override {
        if (event->type() == QEvent::WindowStateChange) {
            if (this->windowState() & Qt::WindowFullScreen) {
                qDebug() << "Window full screen";
                // Handle maximized state
                handleUserManualFullScreen();
            } else if(this->windowState() & Qt::WindowMaximized){
                qDebug() << "Window maximized";
                handleUserManualMaximized();
            }
            else if (this->windowState() == Qt::WindowNoState) {
                qDebug() << "Window normal";
                handleUserManualUnMaximize();
                // Handle normal state

            }
        }
        QMainWindow::changeEvent(event);
    }
};
#endif // MAINWINDOW_H
