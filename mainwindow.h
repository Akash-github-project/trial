#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QtMultimedia>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <QVideoWidget>
#include <string>
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

    void on_horizontalSlider_Duration_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *Player;
    QVideoWidget *Video;
    qint64 mDuration;
    bool IS_Pause = true;
    bool IS_Muted = false;
    bool isLoading = false;
    bool isChanging = false;
    int currentIndex = 0;
    QString FileName;
    QBuffer *buffer;
    QStringList fileListToLoad = {"part1","part2","part3"};
    QString selectedDirectory;

    const int chunkSize = 1024 * 1024; // 1 MB chunk size (adjust as needed)
    const int bufferThreshold = 10 * 1024 * 1024; // 10 MB buffer threshold

    
    void updateDuration(qint64 Duration);
};
#endif // MAINWINDOW_H
