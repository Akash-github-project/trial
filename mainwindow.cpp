#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <FullScreenControlHoverHandler.h>
#include <QGraphicsVideoItem>
#include <QMetaObject>
#include <QUiLoader>

#define IS_FULL_SCREEN windowState().testFlag(Qt::WindowFullScreen)

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


    ui->horizontalSlider_Volume->setMinimum(0);
    ui->horizontalSlider_Volume->setMaximum(100);
    ui->horizontalSlider_Volume->setValue(0);

    Player->setAudioOutput(audioOutput);
    Player->audioOutput()->setVolume(ui->horizontalSlider_Volume->value());

    ui->horizontalSlider_Duration->setTabletTracking(false);


    //done
    connect(Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    //no change
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::loadVideo);
    connect(Player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::handlePlayPauseButtonState);

    seekbarController = new VideoProgressBarController(ui->horizontalSlider_Duration,0);
    seekbarController->setSliderMaxLimit(timeLimit);
    seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    connect(ui->horizontalSlider_Duration,&QSlider::sliderReleased,this,&MainWindow::on_horizontalSlider_Duration_sliderMoved);
    connect(ui->horizontalSlider_Duration,&QSlider::sliderPressed,this,&MainWindow::slderClicked);
    connect(scene, &QGraphicsScene::sceneRectChanged,this,&MainWindow::fullScreenChnaged);
    handler = new EncryptionHandler();
    // make button round
}

void MainWindow::slderClicked(){
    qDebug()<<"---- clicked";
    if(IS_FULL_SCREEN){
        fsSeekbarController->jumpInstantly(this);
    }else {
        seekbarController->jumpInstantly(this);
    }
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
    if(IS_FULL_SCREEN && fsSeekbar != nullptr){
        fsSeekbarController->setSliderMaxLimit(timeLimit);
    }else {
        seekbarController->setSliderMaxLimit(timeLimit);
    }
}

void MainWindow::positionChanged(qint64 duration)
{
    qint64 moveTo =  (currentIndex * timeLimit) + duration / 1000;
    if(IS_FULL_SCREEN){
        if(!fsSeekbar->isSliderDown()) {
            fsSeekbarController->moveSlider(moveTo);
        }
    }else {
        if (!ui->horizontalSlider_Duration->isSliderDown())
        {
        seekbarController->moveSlider(moveTo);
        }
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

        if(IS_FULL_SCREEN && fsSeekbar != nullptr && fsTotalTime != nullptr && fsCurrentTime != nullptr){
            fsTotalTime->setText(TotalTime.toString(Format));
            fsCurrentTime->setText(CurrentTime.toString(Format));
        }else {
            ui->label_current_Time->setText(CurrentTime.toString(Format));
            ui->label_Total_Time->setText(TotalTime.toString(Format));
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    selectedDirectory = QFileDialog::getExistingDirectory(nullptr, "Select Directory", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir directory(selectedDirectory);
    QString filter = videoFileChunkPattern;
    QStringList files = directory.entryList(QStringList() << filter,QDir::Files);
    fileCount = files.length();

    /////
    // Create a QGraphicsScene
    scene = new QGraphicsScene();
    view = new QGraphicsView(scene);

    // Create a QGraphicsVideoItem
    videoItem = new QGraphicsVideoItem();
    scene->addItem(videoItem);

    watermarkItem = new QGraphicsTextItem("Your Watermark Text");
    QFont font("Arial", 24, QFont::Bold);
    watermarkItem->setFont(font);
    watermarkItem->setDefaultTextColor(Qt::black);
    watermarkItem->setPos(10, 10); // Position the watermark
    scene->addItem(watermarkItem);

    ///
    Player->setVideoOutput(videoItem);
    ///
    /// \brief file
    ///
    QFile file(directory.filePath(files.first()));
    file.open(QIODevice::ReadOnly);
    QByteArray videoArray = file.readAll();
    openParticularChunk(videoArray);

    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setBackgroundBrush(Qt::white);
    view->setFrameShape(QFrame::NoFrame);
    view->fitInView(videoItem, Qt::KeepAspectRatio);
    view->setParent(ui->video_section);

    view->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
    videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));

    view->show();

    QObject::disconnect(seekbarConnection);
    seekbarController = new VideoProgressBarController(ui->horizontalSlider_Duration,9);
    seekbarController->setSliderMaxLimit(timeLimit);
    seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    Player->pause();
}

void MainWindow::on_pushButton_Play_Pause_clicked()
{
    if(Player->playbackState() == QMediaPlayer::PlaybackState::PlayingState){
        Player->pause();
    }else {
        Player->play();
    }
}


void MainWindow::on_pushButton_Stop_clicked()
{
    mediaStopped = true;
    Player->stop();
    sliderTime = -1;
    currentIndex = 0;
    IS_Pause = true;
    if(IS_FULL_SCREEN){
        fsSeekbarController->moveSlider(0);
    }else {
        seekbarController->moveSlider(0);
    }
}

void MainWindow::on_pushButton_Volume_clicked()
{
    if (IS_Muted == false)
    {
        IS_Muted = true;
        Player->audioOutput()->setMuted(true);
    }
    else
    {
        IS_Muted = false;
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
    if(IS_FULL_SCREEN){
        fsSeekbarController->moveSlider(seekbarController->horizontalSlider_Duration->value() - 10);
        jumpToPosition(fsSeekbarController->getSliderPosition());
    }else {
        seekbarController->moveSlider(seekbarController->horizontalSlider_Duration->value() - 10);
        jumpToPosition(seekbarController->getSliderPosition());
    }
}


void MainWindow::on_pushButton_Seek_Forward_clicked()
{
    if(IS_FULL_SCREEN){
        fsSeekbarController->moveSlider(seekbarController->horizontalSlider_Duration->value() + 10);
        jumpToPosition(fsSeekbarController->getSliderPosition());
    }else {
        seekbarController->moveSlider(seekbarController->horizontalSlider_Duration->value() + 10);
        jumpToPosition(seekbarController->getSliderPosition());
    }
}

void MainWindow::loadVideo(QMediaPlayer::MediaStatus status){

    if(status == QMediaPlayer::MediaStatus::BufferedMedia && mediaStopped == false){
        Player->pause();
        Player->play();
    }

    if(status == QMediaPlayer::MediaStatus::LoadedMedia && mediaStopped == false){
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

    int position = 0;
    if(IS_FULL_SCREEN){
        position = fsSeekbarController->getValue();
    }else {
        position = seekbarController->getValue();
    }

    qDebug()<<"value of slider "<< position;
    jumpToPosition(position);

 }

 void MainWindow::on_horizontalSlider_Duration_sliderMoved(){
     qDebug()<<"slided----";
     if(IS_FULL_SCREEN){
        fsSeekbarController->setupTimer(this);
     }else {
        seekbarController->setupTimer(this);
     }
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

 void MainWindow::resizeEvent(QResizeEvent* event)
 {
     QMainWindow::resizeEvent(event);
     if(videoItem != nullptr && view != nullptr){
        videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));
        view->fitInView(videoItem, Qt::KeepAspectRatio);
        view->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
     }
 }


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

 void MainWindow::fullScreenChnaged(const QRectF &rect){

     //view->fitInView(rect);
 }


 void MainWindow::on_pushButton_clicked()
 {

     this->setWindowState(Qt::WindowMaximized);
     ui->controlsSection->hide();
     videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     view->fitInView(videoItem, Qt::KeepAspectRatio);
     view->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
     this->setWindowState(Qt::WindowFullScreen);
     view->showFullScreen();
     videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     view->fitInView(videoItem, Qt::KeepAspectRatio);
     view->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
     watermarkItem->setPos(100,100);
     setupFullScreenControls();
 }

 void MainWindow::setupFullScreenControls(){
     QUiLoader loader;
     QFile file(":/controls_overlay.ui"); // Ensure the path to your .ui file is correct
     file.open(QFile::ReadOnly);
     controls = loader.load(&file);
     file.close();

     // Find the controls in the loaded UI

     // Add the UI as a proxy widget to the scene
     QGraphicsProxyWidget *proxy = scene->addWidget(controls);
     //proxy->setPos(0, ui->centralwidget->height() - 300); // Adjust position as necessary
     proxy->setGeometry(QRect(0,ui->centralwidget->height() - 200,ui->centralwidget->width(),200));
     //proxy->setOpacity(0);

     fsPlayPauseButton = controls->findChild<QPushButton*>("fs_PushButton_Play_Pause",Qt::FindChildOption::FindChildrenRecursively);
     fsStopButton = controls->findChild<QPushButton*>("fs_PushButton_Stopp",Qt::FindChildOption::FindChildrenRecursively);
     fsSeekbar = controls->findChild<QSlider*>("fs_HorizontalSlider_Duration",Qt::FindChildOption::FindChildrenRecursively);
     fsNormalButton = controls->findChild<QPushButton*>("normal_screen_button",Qt::FindChildOption::FindChildrenRecursively);
     fsTotalTime = controls->findChild<QLabel*>("fs_label_total_time",Qt::FindChildOption::FindChildrenRecursively);
     fsCurrentTime = controls->findChild<QLabel*>("fs_label_current_time",Qt::FindChildOption::FindChildrenRecursively);
     fsTenSecForward = controls->findChild<QPushButton*>("fs_PushButton_Seek_Forward",Qt::FindChildOption::FindChildrenRecursively);
     fsTenSecBackward = controls->findChild<QPushButton*>("fs_PushButton_Seek_Backward",Qt::FindChildOption::FindChildrenRecursively);
     // Connect the buttons to the media player
     /////
     /// \brief connect
     QGraphicsRectItem* hoverArea = new FullScreenControlHoverHandler(proxy, QRectF(0,ui->centralwidget->height() - 200,ui->centralwidget->width(),200));
     hoverArea->setPen(Qt::NoPen);
     //hoverArea->setBrush(Qt::transparent);
     hoverArea->setAcceptHoverEvents(true);
     scene->addItem(hoverArea);

     ////
     //FullScreenControlHoverHandler* hoverHandler = new FullScreenControlHoverHandler(proxy);
     ///
     connect(fsPlayPauseButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_Play_Pause_clicked);
     connect(fsStopButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_Stop_clicked);
     connect(fsNormalButton,&QPushButton::clicked,this, &MainWindow::on_normal_button_pressed);
     ///

     ///////
    fsSeekbarController = new VideoProgressBarController(fsSeekbar,fileCount);
    fsSeekbarController->setSliderMaxLimit(timeLimit);
    fsSeekbarConnection = connect(fsSeekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    connect(fsSeekbar,&QSlider::sliderReleased,this,&MainWindow::on_horizontalSlider_Duration_sliderMoved);
    connect(fsSeekbar,&QSlider::sliderPressed,this,&MainWindow::slderClicked);
    connect(fsTenSecForward, &QPushButton::clicked,this,&MainWindow::on_pushButton_Seek_Forward_clicked);
    connect(fsTenSecBackward, &QPushButton::clicked,this,&MainWindow::on_pushButton_Seek_Backward_clicked);
 }


 void MainWindow::on_normal_button_pressed(){
     scene->removeItem(controls->graphicsProxyWidget());
     this->setWindowState(Qt::WindowNoState);
     ui->controlsSection->show();
     videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));
     view->fitInView(videoItem, Qt::KeepAspectRatio);
     view->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
     videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));
     view->fitInView(videoItem, Qt::KeepAspectRatio);
     view->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());

     QObject::disconnect(fsSeekbarConnection);
     QObject::disconnect(fsPlayPauseButton);
     QObject::disconnect(fsStopButton);
     QObject::disconnect(fsNormalButton);
     QObject::disconnect(fsSeekbar);
 }

 void MainWindow::handlePlayPauseButtonState(QMediaPlayer::PlaybackState playbackState){
     if(IS_FULL_SCREEN){
         if(playbackState == QMediaPlayer::PlaybackState::PlayingState){
            fsPlayPauseButton->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-image: url(://pause_button);"
                "    background-repeat: no-repeat;"
                "    background-position: center;"
                "}"
             );
         } else {
            fsPlayPauseButton->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-image: url(://play_button);"
                "    background-repeat: no-repeat;"
                "    background-position: center;"
                "}"
             );
         }
     }else {
         if(playbackState == QMediaPlayer::PlaybackState::PlayingState){
            ui->pushButton_Play_Pause->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-image: url(://pause_button);"
                "    background-repeat: no-repeat;"
                "    background-position: center;"
                "}"
             );
         } else {
            ui->pushButton_Play_Pause->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-image: url(://play_button);"
                "    background-repeat: no-repeat;"
                "    background-position: center;"
                "}"
             );
         }
     }
 }
