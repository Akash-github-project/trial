#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videoprogressbarcontroller.h"
#include "encryptionhandler.h"
#include "windoweventhandler.h"
#include "reddotrecording.h"
#include "screenflashlayer.h"
#include "apimanager.h"
#include "nointernetdialog.h"
#include "mizushirushihandora.h"

#include <QMainWindow>
#include <QMediaPlayer>
#include <QtMultimedia>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <QVideoWidget>
#include <QPixmap>
#include <iostream>
///
#include <QFile>
#include <QBuffer>
#include <QDir>
//#include <QGraphicsVideoItem>
#include <PlaybackRateHandler.h>
#include <SeekbarProgressController.h>
#include <customseekbar.h>
#include <fullscreenviews.h>
#include <QWindow>
#include <playercontrollerwidget.h>
#include <warningdialog.h>
#include "CustomGraphicsView.h"
#include <FullScreenControlHoverHandler.h>
#include <fullscreencontrolswidget.h>
//#include <QGraphicsVideoItem>
#include <QMetaObject>
#include <QUiLoader>
#include <QScreen>
#include <apimanager.h>
#include <QMessageBox>
#include <QStackedLayout>
#include <userplaybacktimertracker.h>
#include "customgesturewidget.h"
// platform specific
#include <windows.h>
#include <memory>
#include <comdef.h>
#include <wbemidl.h>
#include <setupapi.h>
#include <screendetector.h>
#include <vlcplayer.h>
// #include <SecureMemory.h>

#define NAME_SIZE 128
#pragma comment(lib, "setuplib.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "wbemuuid.lib")

//#define WATERMARK_TEXT random_video
//#define WATERMARK_TEXT1 random_video_d
#define RECORDING_RED_DOT random_seekbar
#define RECORDING_FLASH_LAYER random_seekbar_slider

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //MainWindow(QWidget *parent = nullptr);
    MainWindow(QString filePath,QString token,QString course_id,QString video_id,QString video_item_id,QString identifier,QString deviceId,QWidget *parent = nullptr);
    ~MainWindow();
    // void makeButtonRound(QPushButton* button);
    struct MonitorInfo {
        std::string deviceName;
        DEVMODE devMode;
    };
    void resizeEvent(QResizeEvent *event);
    void slderClicked(int action);
    void setupFullScreenControls();
    void onPlaybackRateChanged(float playbackRate);
    int getVideoIndexToJump(int timeInSeconds);
    void onlyUpdatePlaybackTimeText(QString playbackDurationString);
    void keyPressEvent(QKeyEvent *event) override;
    void closeNoInternetDialogAndRetry(bool closeWindow);
    void changeVolumeIconToMute();
    void changeVolumeIconToLowFromMute();

public:
    const GUID GUID_CLASS_MONITOR = {0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};
    //PlayerControllerWidget *scene;
    MizuShirushiHandora* watermarkHandler;
    //QMediaPlayer *Player;
    VLCPlayer *Player;
    // Create a QGraphicsScene
    //CustomGraphicsView *view = nullptr;
    //QGraphicsVideoItem *videoItem = nullptr;
    PlaybackRateHandler *playbackRateHandler;
    WarningDialog * warningDialog;
    UserPlaybackTimerTracker * playbackTimer;
    VideoWidget* widgetVideo;

public:
    const int timeLimit = 120;
    int fileCount = 0;
    int minuteRatio = 2;
    int oldHeight = 0;
    int oldWidth = 0;
    int durationAverageInSeconds = 60;
    qint64 lastForward = 0;
    qint64 lastBackward = 0;
    int detectMonitors();
    void enumerateDisplays();
    bool userAction = false;

    bool GetSizeForDevID(short &WidthMm, short &HeightMm);
    bool GetMonitorSizeFromEDID(const HKEY hDevRegKey, short &WidthMm, short &HeightMm);
    QList<QPair<short, short>> getAllMonitorSizes();
    int getSumOfAllVideosTimeTillNow(int index);
    void caliberateVideo();
private:
    // SecureMemory * mem = nullptr;
public slots:
    void onStopClicked();
    void userPlaytimeDataFailed(QString message);
    void userPlaytimeDataSuccess();
    void sendTimeToServer(qint64 playTimeInSeconds);
    void fullScreenChnaged(const QRectF &rect);
    void on_normal_button_pressed();
    void handlePlayPauseButtonState(PlayPauseState playbackState);
    void handleVolumeChange(int volume);
    void handleWindowModesTransitions(bool isFullScreen);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 duration);
    void on_actionOpen_triggered(MizuConfig *config);
    void on_pushButton_Play_Pause_clicked();
    void on_pushButton_Stop_clicked();
    void on_pushButton_Volume_clicked();
    void on_horizontalSlider_Volume_valueChanged(int value);
    void on_pushButton_Seek_Backward_clicked();
    void on_pushButton_Seek_Forward_clicked();
    void loadVideo(PlaybackState status);
    void on_horizontalSlider_Duration_sliderMoved();
    void onSliderStop();
    void on_pushButton_1x_clicked();
    void on_pushButton_1p2x_clicked();
    void on_pushButton_1p5x_clicked();
    void on_pushButton_2x_clicked();
    void on_pushButton_full_screen_clicked();
    void onKeyFetchCompleted(QList<VideoData> keyList, MizuConfig * config, int fullVideoDuration);
    void onNoInternet(QString message);
    QStringList getFileList(const QString& directoryPath);
    void closeWatchTimeNoInternetDialogAndRetry(bool closeWindow);
private:
    bool IS_Pause = true;
    bool IS_Muted = false;
    bool isLoading = false;
    bool isChanging = false;
    bool blockedForPiracy = false;
    bool videoStarted = false;
    bool isFullScreen = false;
    bool mediaStopped = false;
    bool isPaused = false;
    bool pausedForNoInternet = false;
    int extraSeekValue = -1;
    int currentIndex = 0;

    Ui::MainWindow *ui;
    ApiManager * manager;
    QList<VideoData> videoItemList;
    QGuiApplication* guiInstance;
    QCoreApplication* guiApp;
    //QVideoWidget *Video = nullptr;
    //QGraphicsWidget *GraphicsWidget = nullptr;
    //FullScreenViews  *fullScreenViews = nullptr;
    SeekbarProgressController *seekbarNewController;
    // Create a QGraphicsVideoItem
    QList<qint64> videoTimeArray;
    qint64 fullVideoDuration = 0;
    qint64 mDuration;
    QString video_id = "";
    QString course_id = "";
    QString token = "";
    QString video_item_id = "";
    QString identifier = "";
    QString FileName;
    QString folderPath = nullptr;
    QString selectedDirectory;
    QString videoFileChunkPattern = "*.enc";
    QStackedLayout* videoStackedLayout;
    //
    QWidget* videoView;
    QWidget* watermarkPrimary;
    QWidget* watermarkSecondary;
    QWidget* watermarkFlash;
    QWidget* watermarkDot;
    FullScreenControlsWidget* controlsWrapper;
    //QString videoFileChunkPattern = "encrypted*.mp4";
    //qint64 sliderTime = -1;
    VideoProgressBarController *seekbarController = nullptr;
    EncryptionHandler *handler = nullptr;
    //QGraphicsTextItem *WATERMARK_TEXT = nullptr;
    //QGraphicsTextItem *WATERMARK_TEXT1 = nullptr;

    RedDotRecording *RECORDING_RED_DOT = nullptr;
    ScreenFlashLayer *RECORDING_FLASH_LAYER = nullptr;
    QMetaObject::Connection seekbarConnection ;
    std::string phoneNumber = "828282828228";
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
    QComboBox *fsSpeedBox = nullptr;
    QLabel *fsPlaybackLabel = nullptr;
    CustomSeekbar *fsSeekbar = nullptr;
    QSlider *fsSeekbarVolume = nullptr;
    QLabel *fsCurrentTime = nullptr;
    QLabel *fsTotalTime = nullptr;
    QLabel *fsVolumeLowIcon = nullptr;
    QMetaObject::Connection fsSeekbarConnection;
    QMetaObject::Connection fsSeekbarForwardBackwardConnection;
    QWidget *controls;
    WindowEventHandler *fullScreenEventHandler;
    NoInternetDialog *noIntentDialog = nullptr;
    NoInternetDialog *noIntentDialogForTimer = nullptr;
    CustomGestureWidget* wMarkScreen;
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
     QString muteStyle =
                "QLabel {"
                "    border: none;"
                "    background-image: url(://mute_volume);"
                "    background-repeat: no-repeat;"
                "    background-position: center;"
                "}";

     QString lowVolumeStyle =
                "QLabel {"
                "    border: none;"
                "    background-image: url(://volume-low);"
                "    background-repeat: no-repeat;"
                "    background-position: center;"
                "}";

     qint64 lastDisplayTime = 0;
private:
     /////////////////
    void updateDuration(qint64 Duration);
    void loadParticalarChunk(int videoIndex, int extraSeek);
    void jumpToPosition(int secondToJump);
    void openParticularChunk(QByteArray byteData,int videoIndex);
    void windowStateChange(MainWindow *, int, MainWindow *);
    void handleUserManualFullScreen();
    void seekToRemainingTime(long oldTime);
    void setupKeyboardShortcuts();
    void handleUserManualMaximized();
    void handleUserManualUnMaximize();
    void handleArrowKey(int key);
    void disableScreenRecording();
    void showWarningDialog();
    int getExtraSeek(int timeInSeconds, int indexToJump);
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
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override {
        MSG *msg = static_cast<MSG*>(message); // Correctly cast message to MSG*

        if (msg->message == WM_DISPLAYCHANGE) {
            // When display configuration changes, detect monitors again
            auto allMonitors = getAllMonitorSizes();
            qDebug() << "Display change detected (monitor added or removed).";
            if(allMonitors.length() > 1){
                showWarningDialog();
            }
            return true; // Indicate the event is handled
        }

        // Call the base class if the message is not handled
        return QMainWindow::nativeEvent(eventType, message, result);
    }

    void closeEvent(QCloseEvent * event) override{
        if(!videoTimeArray.isEmpty()){
            qint64 currentSeekbarPostion = getCurrentSeekabrPosition();
            qint64 playTimeInSeconds = playbackTimer->getTotalPlayTime();
            if(currentSeekbarPostion != 0 && playTimeInSeconds != 0){
                manager->sendUserWatchTime(token,course_id,video_item_id,video_id,playTimeInSeconds,currentSeekbarPostion);
            }
        }
        event->accept();
    }

    qint64 getCurrentSeekabrPosition();



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
                //handleUserManualUnMaximize();
                // Handle normal state
            }
            else if (this->windowState() & Qt::WindowMinimized) {
                if(Player != nullptr && Player->isPlaying()){
                    on_pushButton_Play_Pause_clicked();
                }
            }
        }
        QMainWindow::changeEvent(event);
    }

    bool event(QEvent *event) override;
private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);
};

#endif // MAINWINDOW_H
