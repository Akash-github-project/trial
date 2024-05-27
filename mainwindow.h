#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videoprogressbarcontroller.h"
#include "encryptionhandler.h"
#include "windoweventhandler.h"

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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void makeButtonRound(QPushButton* button);
    void resizeEvent(QResizeEvent *event);
    void slderClicked();
    void setupFullScreenControls();
public slots:
    void fullScreenChnaged(const QRectF &rect);
    void on_normal_button_pressed();
    void handlePlayPauseButtonState(QMediaPlayer::PlaybackState playbackState);
    void handleVolumeChange(int volume);
    void handleWindowModesTransitions(bool isFullScreen);
private slots:
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

    //void on_horizontalSlider_Duration_sliderMoved();

    void onSliderStop();

    // void on_comboBox_textActivated(const QString &arg1);

    void on_pushButton_1x_clicked();

    void on_pushButton_1p2x_clicked();

    void on_pushButton_1p5x_clicked();

    void on_pushButton_2x_clicked();

    void on_pushButton_full_screen_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *Player;
    QVideoWidget *Video = nullptr;
    QGraphicsWidget *GraphicsWidget = nullptr;
    // Create a QGraphicsScene
    QGraphicsScene *scene;
    QGraphicsView *view = nullptr;

    // Create a QGraphicsVideoItem
    QGraphicsVideoItem *videoItem = nullptr;


    qint64 mDuration;
    bool IS_Pause = true;
    bool IS_Muted = false;
    bool isLoading = false;
    bool isChanging = false;
    int currentIndex = 0;
    QString FileName;
    QBuffer *buffer;
    VideoProgressBarController *seekbarController = nullptr;
    VideoProgressBarController *fsSeekbarController = nullptr;
    QStringList fileListToLoad = {"part1","part2","part3"};
    QString selectedDirectory;
    QString videoFileChunkPattern = "encrypted_input*.mp4";
    qint64 sliderTime = -1;
    EncryptionHandler *handler = nullptr;
    QGraphicsTextItem *watermarkItem = nullptr;
    QMetaObject::Connection seekbarConnection ;
    PlaybackRateHandler *playbackRateHandler;
    const int timeLimit = 120;
    int fileCount = 0;
    int extraSeekValue = -1;
    ///////////// full screen controls
     bool isFullScreen = false;
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
     int oldHeight = 0;
     int oldWidth = 0;
     /////////////////
     /// \brief updateDuration
     /// \param Duration
     ///
    void updateDuration(qint64 Duration);
    void loadParticalarChunk(int videoIndex, int extraSeek);
    void jumpToPosition(int secondToJump);
    void openParticularChunk(QByteArray byteData);
    void onPlaybackRateChanged(float playbackRate);
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
    void windowStateChange(MainWindow *, int, MainWindow *);

    void handleUserManualFullScreen();
    void seekBackward(long oldTime);
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
                qDebug() << "Window maximized";
                // Handle maximized state
                handleUserManualFullScreen();
            } else if (this->windowState() == Qt::WindowNoState) {
                qDebug() << "Window returned to normal state";
                // Handle normal state

            }
        }
        QMainWindow::changeEvent(event);
    }
};
#endif // MAINWINDOW_H
