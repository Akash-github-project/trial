#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Player = new QMediaPlayer();
    QAudioOutput *audioOutput = new QAudioOutput();

    ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButton_Seek_Backward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pushButton_Seek_Forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    ui->horizontalSlider_Volume->setMinimum(0);
    ui->horizontalSlider_Volume->setMaximum(100);
    ui->horizontalSlider_Volume->setValue(30);

    Player->setAudioOutput(audioOutput);
    Player->audioOutput()->setVolume(ui->horizontalSlider_Volume->value());

    connect(Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    //connect(Player, &QMediaPlayer::bufferProgressChanged,this, &MainWindow::handleBufferStatus);
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::loadVideo);

    ui->horizontalSlider_Duration->setRange(0, (5*timeLimit));
    connect(ui->horizontalSlider_Duration,&QSlider::sliderReleased,this,&MainWindow::on_horizontalSlider_Duration_sliderMoved);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete buffer;
}

void MainWindow::durationChanged(qint64 duration)
{
    mDuration = (5*timeLimit);
    ui->horizontalSlider_Duration->setMaximum(mDuration);
}

void MainWindow::positionChanged(qint64 duration)
{
    if (!ui->horizontalSlider_Duration->isSliderDown())
    {
        ui->horizontalSlider_Duration->setValue((currentIndex * timeLimit) + duration / 1000);
    }
    updateDuration((currentIndex * timeLimit) + duration / 1000);
}

void MainWindow::updateDuration(qint64 Duration)
{
    if (Duration || mDuration)
    {
        QTime CurrentTime((Duration / 3600) % (timeLimit / 2), (Duration / 60) % (timeLimit /2), Duration % (timeLimit /2), (Duration * 1000) % 1000);
        QTime TotalTime((mDuration / 3600) % (timeLimit / 2), (mDuration / 60) % (timeLimit /2), mDuration % (timeLimit/2), (mDuration * 1000) % 1000);
        QString Format ="";
        if (mDuration > 3600) Format = "hh:mm:ss";
        else Format = "mm:ss";

        ui->label_current_Time->setText(CurrentTime.toString(Format));
        ui->label_Total_Time->setText(TotalTime.toString(Format));
    }
}

void MainWindow::on_actionOpen_triggered()
{
    selectedDirectory = QFileDialog::getExistingDirectory(nullptr, "Select Directory", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir directory(selectedDirectory);
    QString filter = videoFileChunkPattern;
    QStringList files = directory.entryList(QStringList() << filter,QDir::Files);
    fileCount = files.length();

    Video = new QVideoWidget();
    Video->setGeometry(5, 5, ui->groupBox_Video->width() - 10, ui->groupBox_Video->height() - 10);
    Video->setParent(ui->groupBox_Video);
    Player->setVideoOutput(Video);
    ///
    /// \brief file
    ///
    QFile file(directory.filePath(files.first()));
    file.open(QIODevice::ReadOnly);
    QByteArray videoArray = file.readAll();
    openParticularChunk(videoArray);
    Video->setVisible(true);
    Video->show();
}

void MainWindow::on_pushButton_Play_Pause_clicked()
{
    if (IS_Pause == true)
    {
        IS_Pause = false;
        Player->play();
        ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
    else
    {
        IS_Pause = true;
        Player->pause();
        ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}


void MainWindow::on_pushButton_Stop_clicked()
{
    Player->stop();
    sliderTime = -1;
}


void MainWindow::on_pushButton_Volume_clicked()
{
    if (IS_Muted == false)
    {
        IS_Muted = true;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        Player->audioOutput()->setMuted(true);
    }
    else
    {
        IS_Muted = false;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        Player->audioOutput()->setMuted(false);
    }
}


void MainWindow::on_horizontalSlider_Volume_valueChanged(int value)
{
    if(Player != NULL && Player->audioOutput() != NULL){
        Player->audioOutput()->setVolume(value);
    }
}


void MainWindow::on_pushButton_Seek_Backward_clicked()
{
    ui->horizontalSlider_Duration->setValue(ui->horizontalSlider_Duration->value() - 20);
    Player->setPosition(ui->horizontalSlider_Duration->value() * 1000);
}


void MainWindow::on_pushButton_Seek_Forward_clicked()
{
    ui->horizontalSlider_Duration->setValue(ui->horizontalSlider_Duration->value() + 20);
    Player->setPosition(ui->horizontalSlider_Duration->value() * 1000);
}

void MainWindow::loadVideo(QMediaPlayer::MediaStatus status){


    if(status == QMediaPlayer::MediaStatus::BufferedMedia){
        Player->pause();
        Player->play();
    }

    if(status == QMediaPlayer::MediaStatus::LoadedMedia){
        Player->pause();
        Player->play();
    }


     if(status != QMediaPlayer::MediaStatus::EndOfMedia){
        return;
    } else {
        currentIndex++;
    }

    if(fileCount <= currentIndex) return;
    else qDebug() << "passed 2st test";

    QDir directory(selectedDirectory);
    QStringList filesToPlay = getFileList(selectedDirectory);
    QFile file(directory.filePath(filesToPlay[currentIndex]));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading:" << file.errorString();
        return; // or handle the error in some way
    }

    QByteArray videoArray = file.readAll();
    file.close(); // Close the file after reading
    openParticularChunk(videoArray);

}

 QStringList MainWindow::getFileList(const QString& directoryPath) {
    QDir directory(directoryPath);
    // Set the filter to match files starting with "part" and ending with ".mp4"
    QStringList filters;
    filters << videoFileChunkPattern;

    directory.setNameFilters(filters);
    // Get the list of files
    QStringList fileList = directory.entryList(QDir::Files);
    return fileList;
}
 void MainWindow::on_horizontalSlider_Duration_sliderMoved(){
     if(sliderTime == -1){
        sliderTime = QDateTime::currentMSecsSinceEpoch();
     }
     qint64 difference = QDateTime::currentMSecsSinceEpoch() - sliderTime;
     qDebug() << difference;
     if(difference >= 40){
        int position = ui->horizontalSlider_Duration->sliderPosition();
        jumpToPosition(position);
     }
     sliderTime = QDateTime::currentMSecsSinceEpoch();
 }


 void MainWindow::jumpToPosition(int secondToJump){
     int videoIndexToJump = secondToJump / timeLimit;
     int extraSecondsSeek = secondToJump % timeLimit;
     loadParticalarChunk(videoIndexToJump,extraSecondsSeek);
 }

 void MainWindow::loadParticalarChunk(int videoIndex,int extraSeek){
    if(fileCount <= currentIndex) return;

    else qDebug() << "passed 2st inside "<< videoIndex << " " << currentIndex;

    if(currentIndex == videoIndex){
        Player->setPosition( extraSeek * 1000);
        return;
    }else {
        currentIndex = videoIndex;
    }

    QDir directory(selectedDirectory);
    QStringList filesToPlay = getFileList(selectedDirectory);
    QFile file(directory.filePath(filesToPlay[videoIndex]));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading:" << file.errorString();
        return; // or handle the error in some way
    }

    QByteArray videoArray = file.readAll();
    file.close(); // Close the file after reading
    openParticularChunk(videoArray);
    Player->setPosition(extraSeek * 1000);

 }

 void MainWindow::openParticularChunk(QByteArray byteData){
    QBuffer* newBuffer;
    // Create a new buffer and set its data
    newBuffer = new QBuffer();
    newBuffer->setData(byteData);

    // Open the buffer for reading
    if (!newBuffer->open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open buffer for reading";
        return; // or handle the error in some way
    }
    qDebug() << "here";
    // Set the source device for the player
    Player->setSourceDevice(newBuffer, QUrl("someelse.mp4"));
    qDebug() << "after here";
 }


 void MainWindow::on_groupBox_Video_clicked()
 {

 }

