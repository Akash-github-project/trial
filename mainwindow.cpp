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

    ui->horizontalSlider_Duration->setRange(0, (5*60+53));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete buffer;
}

void MainWindow::durationChanged(qint64 duration)
{
    // mDuration = duration / 1000;
    // ui->horizontalSlider_Duration->setMaximum(mDuration);

    mDuration = (5*60) + 53;
    ui->horizontalSlider_Duration->setMaximum(mDuration);
}

void MainWindow::positionChanged(qint64 duration)
{
    if (!ui->horizontalSlider_Duration->isSliderDown())
    {
        ui->horizontalSlider_Duration->setValue((currentIndex * 60) + duration / 1000);
    }
    updateDuration((currentIndex * 60) + duration / 1000);
}

void MainWindow::updateDuration(qint64 Duration)
{
    if (Duration || mDuration)
    {
        QTime CurrentTime((Duration / 3600) % 60, (Duration / 60) % 60, Duration % 60, (Duration * 1000) % 1000);
        QTime TotalTime((mDuration / 3600) % 60, (mDuration / 60) % 60, mDuration % 60, (mDuration * 1000) % 1000);
        QString Format ="";
        if (mDuration > 3600) Format = "hh:mm:ss";
        else Format = "mm:ss";

        ui->label_current_Time->setText(CurrentTime.toString(Format));
        ui->label_Total_Time->setText(TotalTime.toString(Format));
    }
}

void MainWindow::on_actionOpen_triggered()
{
   //FileName = QFileDialog::getOpenFileName(this, tr("Select Video File"), "", tr("MP4 Files (*.mp4)"));
    // FileName = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    selectedDirectory = QFileDialog::getExistingDirectory(nullptr, "Select Directory", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir directory(selectedDirectory);
    QString filter = "output*.mp4";
    QStringList files = directory.entryList(QStringList() << filter,QDir::Files);


    Video = new QVideoWidget();

    Video->setGeometry(5, 5, ui->groupBox_Video->width() - 10, ui->groupBox_Video->height() - 10);

    Video->setParent(ui->groupBox_Video);

    Player->setVideoOutput(Video);

    ///
    /// \brief file
    ///
    QBuffer* newBuffer;
    QFile file(directory.filePath(files.first()));
    file.open(QIODevice::ReadOnly);
    QByteArray videoArray = file.readAll();
    newBuffer = new QBuffer();
    newBuffer->setData(videoArray);
    newBuffer->open(QIODevice::ReadWrite);
    Player->setSourceDevice(newBuffer,QUrl("some.mp4"));
    //Player->setSource(QUrl(FileName));
    //Player->setMedia();
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
}


void MainWindow::on_pushButton_Volume_clicked()
{
    if (IS_Muted == false)
    {
        IS_Muted = true;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        Player->audioOutput()->setMuted(true);
        //Player->setMuted(true);
    }
    else
    {
        IS_Muted = false;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        Player->audioOutput()->setMuted(false);
        //Player->setMuted(false);
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

    QDir directory(selectedDirectory);
    QStringList filesToPlay = getFileList(selectedDirectory);
    //int fileIndex = (currentIndex * 60 + (duration / 1000)) / 60;
    //qDebug() << "in loadVideo" << duration;
    qDebug() << "Media Status" << status;

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
    // Video->setVisible(false);
    // Video->hide();
    // if((duration / 100) == 5500 && isChanging == false){
    //     //fileIndex = currentIndex + 1;
    //     currentIndex++;
    //     isChanging = true;
    //     qDebug() << "duration: 50000" << duration;
    // }

    // if(isChanging == false) {
    //     return;
    // } else {
    //     qDebug() << "passed 1st test";
    // };


    if(filesToPlay.length() == currentIndex) return;
    else qDebug() << "passed 2st test";

    QFile file(directory.filePath(filesToPlay[currentIndex]));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading:" << file.errorString();
        return; // or handle the error in some way
    }

    QByteArray videoArray = file.readAll();
    file.close(); // Close the file after reading

    // Player->stop();
    // Player->setVideoOutput(Video);
    // Close the existing buffer, if it exists
    QBuffer* newBuffer;

    // Create a new buffer and set its data
    newBuffer = new QBuffer();
    newBuffer->setData(videoArray);

    // Open the buffer for reading
    if (!newBuffer->open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open buffer for reading";
        return; // or handle the error in some way
    }
    qDebug() << "here";

    // Set the source device for the player
    Player->setSourceDevice(newBuffer, QUrl("someelse.mp4"));
    //Player->play();
    // Video->setVisible(true);
    // Video->show();
    // IS_Pause = true;
    // isChanging = false;
    qDebug() << "after here";
    //on_pushButton_Play_Pause_clicked();
}

 QStringList MainWindow::getFileList(const QString& directoryPath) {
    QDir directory(directoryPath);

    // Set the filter to match files starting with "part" and ending with ".mp4"
    QStringList filters;
    filters << "output*.mp4";

    directory.setNameFilters(filters);

    // Get the list of files
    QStringList fileList = directory.entryList(QDir::Files);

    // Print the list of files
    //qDebug() << "Files in directory:" << directoryPath;
    // for (const QString& file : fileList) {
    //     qDebug() << file;
    // }
    return fileList;
}

 void MainWindow::on_horizontalSlider_Duration_sliderMoved(int position)
 {
    Player->setPosition(position * 1000);
 }

