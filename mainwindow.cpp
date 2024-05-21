#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMetaObject>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Player = new QMediaPlayer();
    QAudioOutput *audioOutput = new QAudioOutput();

    ui->pushButton_Play_Pause->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    background-image: url(://play_button);"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "}"
        );

    //ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    //ui->pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    //ui->pushButton_Seek_Backward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    //ui->pushButton_Seek_Forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    // ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    ui->horizontalSlider_Volume->setMinimum(0);
    ui->horizontalSlider_Volume->setMaximum(100);
    ui->horizontalSlider_Volume->setValue(30);

    Player->setAudioOutput(audioOutput);
    Player->audioOutput()->setVolume(ui->horizontalSlider_Volume->value());

    ui->horizontalSlider_Duration->setTabletTracking(false);

    connect(Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::loadVideo);
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::playAnyRemainig);

    seekbarController = new VideoProgressBarController(ui->horizontalSlider_Duration,0);
    seekbarController->setSliderMaxLimit(timeLimit);
    seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    connect(ui->horizontalSlider_Duration,&QSlider::sliderReleased,this,&MainWindow::on_horizontalSlider_Duration_sliderMoved);
    connect(ui->horizontalSlider_Duration,&QSlider::sliderPressed,this,&MainWindow::slderClicked);
    //connect(parent,&QWidget::resize,this,&MainWindow::onResized);
    //seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    handler = new EncryptionHandler();
    // make button round
    //makeButtonRound(ui->pushButton_Play_Pause);
}

void MainWindow::slderClicked(){
    qDebug()<<"---- clicked";
    seekbarController->jumpInstantly(this);
}

void MainWindow::makeButtonRound(QPushButton*  button){
    QRect rect(10,10,50,50);
    qDebug() << rect.size();
    qDebug() << button->size();
    QRegion region(rect, QRegion::Ellipse);
    qDebug() << region.boundingRect().size();
    button->setMask(region);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete buffer;
}

void MainWindow::durationChanged(qint64 duration)
{
    seekbarController->setSliderMaxLimit(timeLimit);
}

void MainWindow::positionChanged(qint64 duration)
{
    qint64 moveTo =  (currentIndex * timeLimit) + duration / 1000;
    if (!ui->horizontalSlider_Duration->isSliderDown())
    {
        seekbarController->moveSlider(moveTo);
        //ui->horizontalSlider_Duration->setValue((currentIndex * timeLimit) + duration / 1000);
    }
    updateDuration(moveTo);
}

void MainWindow::updateDuration(qint64 Duration)
{
    qint64 fullVideoDurtaion = fileCount * timeLimit;
    if (Duration || fullVideoDurtaion)
    {
        QTime CurrentTime((Duration / 3600) % (timeLimit / 2), (Duration / 60) % (timeLimit /2), Duration % (timeLimit /2), (Duration * 1000) % 1000);
        QTime TotalTime((fullVideoDurtaion / 3600) % (timeLimit / 2), (fullVideoDurtaion / 60) % (timeLimit /2), fullVideoDurtaion % (timeLimit/2), (fullVideoDurtaion * 1000) % 1000);
        QString Format ="";
        if (fullVideoDurtaion > 3600) Format = "hh:mm:ss";
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
    Video->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
    Video->setWindowFlag(Qt::FramelessWindowHint);
    Video->setParent(ui->video_section);
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

    QObject::disconnect(seekbarConnection);
    seekbarController = new VideoProgressBarController(ui->horizontalSlider_Duration,9);
    seekbarController->setSliderMaxLimit(timeLimit);
    seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    Video->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    Player->pause();
}

void MainWindow::on_pushButton_Play_Pause_clicked()
{
    if (IS_Pause == true)
    {
        IS_Pause = false;
        Player->play();
        ui->pushButton_Play_Pause->setStyleSheet(
            "QPushButton {"
            "    border: none;"
            "    background-image: url(://pause_button);"
            "    background-repeat: no-repeat;"
            "    background-position: center;"
            "}"
         );
    }
    else
    {
        IS_Pause = true;
        Player->pause();
        ui->pushButton_Play_Pause->setStyleSheet(
            "QPushButton {"
            "    border: none;"
            "    background-image: url(://play_button);"
            "    background-repeat: no-repeat;"
            "    background-position: center;"
            "}"
         );
        //ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}


void MainWindow::on_pushButton_Stop_clicked()
{
    Player->stop();
    sliderTime = -1;
    currentIndex = 0;
    IS_Pause = true;
    ui->pushButton_Play_Pause->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    background-image: url(://play_button);"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "}"
    );
}


void MainWindow::on_pushButton_Volume_clicked()
{
    if (IS_Muted == false)
    {
        IS_Muted = true;
        // ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        Player->audioOutput()->setMuted(true);
    }
    else
    {
        IS_Muted = false;
        // ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
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
    seekbarController->moveSlider(seekbarController->horizontalSlider_Duration->value() - 10);
    jumpToPosition(seekbarController->getSliderPosition());
}


void MainWindow::on_pushButton_Seek_Forward_clicked()
{
    seekbarController->moveSlider(seekbarController->horizontalSlider_Duration->value() + 10);
    jumpToPosition(seekbarController->getSliderPosition());
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

 void MainWindow::onSliderStop(){
    //int position = seekbarController->getSliderPosition();
    int position = seekbarController->getValue();
    qDebug()<<"value of slider "<< position;
    jumpToPosition(position);

 }

 void MainWindow::on_horizontalSlider_Duration_sliderMoved(){
     qDebug()<<"slided----";
     seekbarController->setupTimer(this);
 }


 void MainWindow::jumpToPosition(int secondToJump){
     int videoIndexToJump = secondToJump / timeLimit;
     int extraSecondsSeek = secondToJump % timeLimit;
     loadParticalarChunk(videoIndexToJump,extraSecondsSeek);
 }

 void MainWindow::loadParticalarChunk(int videoIndex,int extraSeek){
    if(fileCount <= currentIndex) return;

    else qDebug() << "passed 2st inside "<< videoIndex << " " << currentIndex << " " << extraSeek;

    if(currentIndex == videoIndex){
        Player->setPosition(extraSeek * 1000);
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

    //2:33:41
    QByteArray videoArray = file.readAll();
    file.close(); // Close the file after reading
    openParticularChunk(videoArray);
    Player->setPosition(extraSeek * 1000);

 }

 void MainWindow::openParticularChunk(QByteArray byteData){
    QBuffer* newBuffer;
    // Create a new buffer and set its data
    newBuffer = new QBuffer();
    newBuffer->setData(handler->decryptFile(byteData));

    // Open the buffer for reading
    if (!newBuffer->open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open buffer for reading";
        return; // or handle the error in some waa
    }
    qDebug() << "here";
    // Set the source device for the player
    Player->setSourceDevice(newBuffer, QUrl("someelse.mp4"));
    qDebug() << "after here";
 }

 void MainWindow::onResized(int w, int h) {
     qDebug() << "Signal-Slot: New size:" << w;
     //label->setText(QString("New size: %1 x %2").arg(newSize.width()).arg(newSize.height()));
 }

 void MainWindow::resizeEvent(QResizeEvent* event)
 {
     QMainWindow::resizeEvent(event);
     // Your code here.
     qDebug()<<"hello there";
     if(Video != nullptr){
        Video->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
     }
 }


 // void MainWindow::on_comboBox_textActivated(const QString &arg1)
 // {
 //     int pos = arg1.lastIndexOf(QChar('x'));
 //     QString rate = arg1.left(pos);
 //     qDebug() << rate;
 //     Player->setPlaybackRate(rate.toDouble());
 // }


 void MainWindow::on_pushButton_1x_clicked()
 {
     Player->setPlaybackRate(1.0);
 }


 void MainWindow::on_pushButton_1p2x_clicked()
 {
     Player->setPlaybackRate(1.2);
 }


 void MainWindow::on_pushButton_1p5x_clicked()
 {
     Player->setPlaybackRate(1.5);
 }


 void MainWindow::on_pushButton_2x_clicked()
 {
     Player->setPlaybackRate(2.0);
 }

 void MainWindow::playAnyRemainig(QMediaPlayer::MediaStatus status){
     qDebug()<<"hello"<<status;
 }



 void MainWindow::on_pushButton_clicked()
 {
     qDebug()<<"full screen";
     Video->setFullScreen(true);
 }
