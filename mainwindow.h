#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videoprogressbarcontroller.h"
#include "encryptionhandler.h"

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
private slots:
    void onResized(int w, int h);
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
    void playAnyRemainig(QMediaPlayer::MediaStatus status);

    QStringList getFileList(const QString& directoryPath);

    void on_horizontalSlider_Duration_sliderMoved();

    //void on_horizontalSlider_Duration_sliderMoved();

    void onSliderStop();

    // void on_comboBox_textActivated(const QString &arg1);

    void on_pushButton_1x_clicked();

    void on_pushButton_1p2x_clicked();

    void on_pushButton_1p5x_clicked();

    void on_pushButton_2x_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *Player;
    QVideoWidget *Video = nullptr;
    qint64 mDuration;
    bool IS_Pause = true;
    bool IS_Muted = false;
    bool isLoading = false;
    bool isChanging = false;
    int currentIndex = 0;
    QString FileName;
    QBuffer *buffer;
    VideoProgressBarController *seekbarController = nullptr;
    QStringList fileListToLoad = {"part1","part2","part3"};
    QString selectedDirectory;
    QString videoFileChunkPattern = "encrypted_chunk_*.mp4";
    qint64 sliderTime = -1;
    EncryptionHandler *handler = nullptr;
    QMetaObject::Connection seekbarConnection ;
    const int timeLimit = 120;
    int fileCount = 0;
    int extraSeekValue = -1;

    
    void updateDuration(qint64 Duration);
    void loadParticalarChunk(int videoIndex, int extraSeek);
    void jumpToPosition(int secondToJump);
    void openParticularChunk(QByteArray byteData);
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p{this};
        p.fillRect(rect(), {100, 100, 100, 128});
        p.setPen({0, 200, 255});
        p.drawText(rect(), "Loading...", Qt::AlignHCenter | Qt::AlignVCenter);
    }
};
#endif // MAINWINDOW_H
