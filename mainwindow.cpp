#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "CustomGraphicsView.h"

#include <FullScreenControlHoverHandler.h>
#include <QGraphicsVideoItem>
#include <QMetaObject>
#include <QUiLoader>
#include <QScreen>

#define IS_FULL_SCREEN windowState().testFlag(Qt::WindowFullScreen)
//#define IS_FULL_SCREEN false

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Player = new QMediaPlayer();
    QAudioOutput *audioOutput = new QAudioOutput();
    seekbarNewController = new SeekbarProgressController(this);
    ui->pushButton_Play_Pause->setStyleSheet( playButtonStyle );


    ui->horizontalSlider_Volume->setMinimum(0);
    ui->horizontalSlider_Volume->setMaximum(100);
    ui->horizontalSlider_Volume->setValue(0);

    Player->setAudioOutput(audioOutput);
    Player->audioOutput()->setVolume(ui->horizontalSlider_Volume->value() / 100.0f);

    ui->horizontalSlider_Duration->setTabletTracking(false);
    playbackRateHandler = new PlaybackRateHandler(this,Player);


    //done
    connect(Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    // connect(seekbarNewController, &SeekbarProgressController::timeMovedTo, this, &MainWindow::positionChanged);
    //no change
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::loadVideo);
    connect(Player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::handlePlayPauseButtonState);

    seekbarController = new VideoProgressBarController(ui->horizontalSlider_Duration,0);
    seekbarController->setSliderMaxLimit(timeLimit);
    seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    connect(ui->horizontalSlider_Duration,&QSlider::sliderReleased,this,&MainWindow::on_horizontalSlider_Duration_sliderMoved);
    connect(ui->horizontalSlider_Duration,&QSlider::actionTriggered,this,&MainWindow::slderClicked);
    connect(ui->horizontalSlider_Volume,&QSlider::valueChanged,this,&MainWindow::handleVolumeChange);
    connect(playbackRateHandler,&PlaybackRateHandler::playbackRateChanged,this,&MainWindow::onPlaybackRateChanged);
    handler = new EncryptionHandler();
    fullScreenEventHandler = new WindowEventHandler(this,this);
    // don't know if we need this class
    connect(fullScreenEventHandler,&WindowEventHandler::onFullScreenStateChanged,this,&MainWindow::handleWindowModesTransitions);
    QCoreApplication* guiApp = QGuiApplication::instance();
    QGuiApplication* instance = qobject_cast<QGuiApplication*>(guiApp);
    QObject::connect(
        instance,
        &QGuiApplication::screenAdded,
        [](){
            qDebug()<<"device added-------------------- warning!!!!!!!!!";
    });
    setupKeyboardShortcuts();
    setFixedSize(1024,768);
    disableScreenRecording();
    //setWindowFlag(Qt::Drawer);
}

void MainWindow::disableScreenRecording(){
    // Get the native window handle
    HWND hwnd = reinterpret_cast<HWND>(winId());

    // Define a mask to restrict to the primary GPU
    DWORD displayAffinity = WDA_EXCLUDEFROMCAPTURE;

    // Set the display affinity
    SetWindowDisplayAffinity(hwnd, displayAffinity);
}

void MainWindow::setupKeyboardShortcuts(){
    QShortcut *playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(playPauseShortcut, &QShortcut::activated, this, &MainWindow::on_pushButton_Play_Pause_clicked);

}

void MainWindow::handleWindowModesTransitions(bool isFullScreen){
    qDebug()<<"full screen";
    //setupFullScreenControls();
}

void MainWindow::seekBackward(long oldTime){
    qDebug()<<"-------------------------";
    qDebug()<<oldTime;
    jumpToPosition(oldTime);
}


void MainWindow::slderClicked(int action){
    qDebug()<<"slider clicked called"<<action<<"slider value"<<seekbarController->getValue();
    // if(IS_FULL_SCREEN){
    //     fsSeekbarController->jumpInstantly(this);
    // }else {
    //     seekbarController->jumpInstantly(this);
    // }
}

void MainWindow::handleVolumeChange(int volume){
    Player->audioOutput()->setVolume(volume / 100.0f);
}

void MainWindow::onPlaybackRateChanged(float playbackRate){
    QString defaultStyles = "QPushButton {"
            "background-color:black;"
            "color:white;"
            "border:1px solid white;"
            "border-radius:4px;"
            "padding:5px 10px;"
            "}"
    ;

    QString selectedStyles =
            "QPushButton {"
            "background-color:#044FC0;"
            "color:white;"
            "border-radius:4px;"
            "padding:5px 10px;"
            "}"
    ;

    if(IS_FULL_SCREEN){
        fsSpeed1x->setStyleSheet(defaultStyles);
        fsSpeed1p2x->setStyleSheet(defaultStyles);
        fsSpeed1p5x->setStyleSheet(defaultStyles);
        fsSpeed2x->setStyleSheet(defaultStyles);

        if(playbackRate == 1.0f){
            fsSpeed1x->setStyleSheet(selectedStyles);
        }else if(playbackRate == 1.2f){
            fsSpeed1p2x->setStyleSheet(selectedStyles);
        }else if(playbackRate == 1.5f){
            fsSpeed1p5x->setStyleSheet(selectedStyles);
        }else if(playbackRate == 2.0f){
            fsSpeed2x->setStyleSheet(selectedStyles);
        }
    }else {
        ui->pushButton_1x->setStyleSheet(defaultStyles);
        ui->pushButton_1p2x->setStyleSheet(defaultStyles);
        ui->pushButton_1p5x->setStyleSheet(defaultStyles);
        ui->pushButton_2x->setStyleSheet(defaultStyles);

        if(playbackRate == 1.0f){
            ui->pushButton_1x->setStyleSheet( selectedStyles );
        }else if(playbackRate == 1.2f){
            ui->pushButton_1p2x->setStyleSheet( selectedStyles );
        }else if(playbackRate == 1.5f){
            ui->pushButton_1p5x->setStyleSheet( selectedStyles );
        }else if(playbackRate == 2.0f){
            ui->pushButton_2x->setStyleSheet( selectedStyles );
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
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
    int sum = 0;
    for(int i= 0;i<currentIndex;i++){
        sum += videoTimeArray[i];
    }
    // qint64 moveTo = (currentIndex * timeLimit) + (qint64)floor(duration / 1000.0) ;
    qint64 moveTo = (sum / 1000) + (duration / 1000) ;
    // qint64 moveTo = duration;

    //qDebug()<<"move to "<<moveTo;
    if(IS_FULL_SCREEN && fsSeekbar != nullptr){
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
    int x = QRandomGenerator::global()->bounded(0,  ui->video_section->height() / 2);
    int y = QRandomGenerator::global()->bounded(0,  ui->video_section->width() - 50);
    if(WATERMARK_TEXT != nullptr && (QDateTime::currentSecsSinceEpoch() - lastDisplayTime > 2 )){
        lastDisplayTime = QDateTime::currentSecsSinceEpoch();
        WATERMARK_TEXT->setPos(x,y);
    }
}

void MainWindow::updateDuration(qint64 Duration)
{
    qint64 fullVideoDurtaion = fileCount * timeLimit;
    int minuteRatio = 2;
    if (Duration || fullVideoDurtaion)
    {
        QTime CurrentTime((Duration / 3600) % (timeLimit / minuteRatio), (Duration / 60) % (timeLimit /minuteRatio), Duration % (timeLimit /minuteRatio), (Duration * 1000) % 1000);
        QTime TotalTime((fullVideoDurtaion / 3600) % (timeLimit / minuteRatio), (fullVideoDurtaion / 60) % (timeLimit /minuteRatio), fullVideoDurtaion % (timeLimit/minuteRatio), (fullVideoDurtaion * 1000) % 1000);
        //qDebug()<<CurrentTime<<"current time";
        //qDebug()<<TotalTime<<"total time";
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
    if(fileCount == 0){
        return;
    }

    /////
    // Create a QGraphicsScene
    scene = new QGraphicsScene();
    view = new CustomGraphicsView();
    view->installEventFilter(this);
    view->setScene(scene);

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create a QGraphicsVideoItem
    videoItem = new QGraphicsVideoItem();
    scene->addItem(videoItem);

    WATERMARK_TEXT = new QGraphicsTextItem("Your Watermark Text");
    QFont font("Arial", 24, QFont::Bold);
    WATERMARK_TEXT->setFont(font);
    WATERMARK_TEXT->setDefaultTextColor(Qt::black);
    WATERMARK_TEXT->setOpacity(0.3f);
    WATERMARK_TEXT->setPos(10, 10); // Position the watermark
    // red recording dot
    scene->addItem(WATERMARK_TEXT);
    RECORDING_RED_DOT = new RedDotRecording(scene,"8787878799",ui->video_section);
    RECORDING_FLASH_LAYER = new ScreenFlashLayer(scene,phoneNumber,ui->video_section);
    Player->setVideoOutput(videoItem);
    RECORDING_RED_DOT->updatePosition(20, view->height());
    RECORDING_FLASH_LAYER->updateSize(0,0,ui->centralwidget->width(),ui->centralwidget->height());

    ///
    /// \brief file
    ///
    QFile file(directory.filePath(files.first()));
    file.open(QIODevice::ReadOnly);
    QByteArray videoArray = file.readAll();
    openParticularChunk(videoArray);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setBackgroundBrush(Qt::black);
    view->setFrameShape(QFrame::NoFrame);
    view->fitInView(videoItem, Qt::KeepAspectRatio);
    view->setParent(ui->video_section);

    view->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
    videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));

    view->show();
    seekbarNewController->startTimer();

    QObject::disconnect(seekbarConnection);
    seekbarController = new VideoProgressBarController(ui->horizontalSlider_Duration,fileCount);
    connect(seekbarController,&VideoProgressBarController::onSeekbarSecondsTimerEndSliding,this,&MainWindow::seekBackward);
    seekbarController->setSliderMaxLimit(timeLimit);
    seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    //Player->pause();
    playbackRateHandler->chnagePlaybackRate(1.0f);
    RECORDING_FLASH_LAYER->startFlasing();
    RECORDING_RED_DOT->startFlasing();
    RECORDING_RED_DOT->updatePosition(20, view->height() - 40);
}

void MainWindow::on_pushButton_Play_Pause_clicked(){
    //qDebug()<<Player->playbackState();
    if(fileCount <= 0) return;
    if(Player->playbackState() == QMediaPlayer::PlaybackState::PlayingState){
        seekbarNewController->pause();
        Player->pause();
    }else {
        if(Player->playbackState() == QMediaPlayer::PlayingState && mediaStopped == true){
            Player->pause();
        }
        seekbarNewController->resume();
        Player->play();
        mediaStopped = false;
        IS_Pause = false;
    }
}


void MainWindow::on_pushButton_Stop_clicked()
{
    loadParticalarChunk(0,0);
    mediaStopped = true;
    Player->pause();
    sliderTime = -1;
    currentIndex = 0;
    IS_Pause = true;
    if(IS_FULL_SCREEN){
        fsSeekbarController->moveSlider(0);
    }else {
        seekbarController->moveSlider(0);
    }
    handlePlayPauseButtonState(QMediaPlayer::PausedState);
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
    if(fileCount <= 0) return;
    if(IS_FULL_SCREEN){
        int oldTime = fsSeekbarController->getValue() - 10;
        int remaingTime = timeLimit - (fsSeekbarController->getValue() % timeLimit);
        if(remaingTime >= 110){
            fsSeekbarController->moveSlider(fsSeekbarController->getValue() - 10);
            jumpToPosition(fsSeekbarController->getValue());
            fsSeekbarController->setupSeekTimer(this,oldTime);
        }else {
            fsSeekbarController->moveSlider(fsSeekbarController->getValue() - 10);
            jumpToPosition(oldTime);
        }

    }else {
        int oldTime = seekbarController->getValue() - 10;
        int remaingTime = timeLimit - (seekbarController->getValue() % timeLimit);
        if(remaingTime >= 110){
            seekbarController->moveSlider(seekbarController->getValue() - 10);
            jumpToPosition(seekbarController->getValue());
            seekbarController->setupSeekTimer(this,oldTime);
        }else {
            seekbarController->moveSlider(seekbarController->getValue() - 10);
            jumpToPosition(oldTime);
        }
    }
}


void MainWindow::on_pushButton_Seek_Forward_clicked()
{
    if(fileCount <= 0) return;
    if(IS_FULL_SCREEN){
        int oldTime = fsSeekbarController->getValue() + 10;
        int remaingTime = timeLimit - (fsSeekbarController->getValue() % timeLimit);
        if(remaingTime <= 10){
            fsSeekbarController->moveSlider(fsSeekbarController->getValue() + 10);
            jumpToPosition(fsSeekbarController->getValue());
            fsSeekbarController->setupSeekTimer(this,oldTime);
        }else {
            fsSeekbarController->moveSlider(fsSeekbarController->getValue() + 10);
            jumpToPosition(oldTime);
        }

    }else {
        int oldTime = seekbarController->getValue() + 10;
        //qDebug()<<"old time"<<oldTime;
        int remaingTime = timeLimit - (seekbarController->getValue() % timeLimit);
        if(remaingTime <= 10){
            seekbarController->moveSlider(seekbarController->getValue() + 10);
            jumpToPosition(seekbarController->getValue());
            seekbarController->setupSeekTimer(this,oldTime);
        }else {
            seekbarController->moveSlider(seekbarController->getValue() + 10);
            jumpToPosition(oldTime);
        }
    }
}

void MainWindow::loadVideo(QMediaPlayer::MediaStatus status){
    //qDebug()<<"media status"<<status;
    if(status == QMediaPlayer::MediaStatus::BufferedMedia && mediaStopped == false){
        Player->play();
        //qDebug()<<Player->duration()<<"media duration";
    }

    if(status == QMediaPlayer::MediaStatus::LoadedMedia && mediaStopped == false){
        Player->play();
    }


     if(status != QMediaPlayer::MediaStatus::EndOfMedia){
        return;
    } else {
        seekbarNewController->pause();
        currentIndex++;
    }

    if(fileCount <= currentIndex) {
        seekbarNewController->stop();
        return;
    }
    //else qDebug() << "passed 2st test" << currentIndex;

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
    seekbarNewController->resume();
    Player->play();

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
    seekbarNewController->markDraggingEnded();
    int position = 0;
    if(IS_FULL_SCREEN){
        position = fsSeekbarController->getValue();
    }else {
        position = seekbarController->getValue();
    }

    //qDebug()<<"value of slider "<< position;
    jumpToPosition(position);

 }

 void MainWindow::on_horizontalSlider_Duration_sliderMoved(){
     //qDebug()<<"slided----";
     if(IS_FULL_SCREEN){
        fsSeekbarController->setupTimer(this);
        seekbarNewController->markGettingDragged();
     }else {
        seekbarController->setupTimer(this);
        seekbarNewController->markGettingDragged();
     }
 }


 void MainWindow::jumpToPosition(int secondToJump){
     //qDebug()<<Player->isSeekable()<< "is seekable";
     //qDebug()<<secondToJump<<"jump second";
     int videoIndexToJump = getVideoIndexToJump(secondToJump);
     int extraSecondsSeek = getExtraSeek(secondToJump,videoIndexToJump); //secondToJump % timeLimit;
     //qDebug()<<"videoIndexToJump "<<videoIndexToJump;
     //qDebug()<<"extraSecondsSeek "<<extraSecondsSeek;
     loadParticalarChunk(videoIndexToJump,extraSecondsSeek);
 }

 int MainWindow::getExtraSeek(int timeInSeconds,int indexToJump){
     int sumToIndex = 0;
     int timeSeconds = 1000 * timeInSeconds;
     for(int i = 0; i< indexToJump;i++){
         sumToIndex += videoTimeArray[i];
     }
     if(timeSeconds == sumToIndex){
         return 0;
     }

     if(timeSeconds > sumToIndex){
         return (timeSeconds - sumToIndex) / 1000;
     }
     return 0;
 }

 int MainWindow::getVideoIndexToJump(int timeInSeconds){
     int difference = 1000 * timeInSeconds;
     int indexToReturn = 0;
     for (int i = 0;i< sizeof(videoTimeArray); i++){
         difference -= videoTimeArray[i];
         if(difference < 0){
             indexToReturn = i;
             break;
         }
         if(difference == 0 && (i == (sizeof(videoTimeArray) - 1))){
             indexToReturn = i;
             break;
         }
         if(difference == 0){
             indexToReturn = i+1;
             break;
         }
     }
     return indexToReturn;
 }

 void MainWindow::loadParticalarChunk(int videoIndex,int extraSeek){

     if(fileCount <= currentIndex) {
         seekbarNewController->pause();
         return;
     }

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
    seekbarController->setupSeekTimer(this,seekbarController->getValue() + extraSeek);

 }


 void MainWindow::openParticularChunk(QByteArray byteData){
    // Create a new buffer and set its data
    QBuffer* newBuffer = new QBuffer();
    newBuffer->setData(handler->decryptFile(byteData));

    // Open the buffer for reading
    if (!newBuffer->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open buffer for reading";
        return; // or handle the error in some waa
    }
    Player->setSourceDevice(newBuffer, QUrl("someelse.mp4"));
    //qDebug() << "after here";
 }

 void MainWindow::resizeEvent(QResizeEvent* event)
 {
     QMainWindow::resizeEvent(event);
     if(videoItem != nullptr && view != nullptr && !IS_FULL_SCREEN){
        videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));
        view->fitInView(videoItem, Qt::KeepAspectRatio);
        view->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
        RECORDING_RED_DOT->updatePosition(  20, view->height() - 40);
        RECORDING_FLASH_LAYER->updateSize(0,0,view->width(),view->height());
     }
 }


 void MainWindow::on_pushButton_1x_clicked()
 {
     playbackRateHandler->chnagePlaybackRate(1.0f);
 }


 void MainWindow::on_pushButton_1p2x_clicked()
 {
     playbackRateHandler->chnagePlaybackRate(1.2f);
 }


 void MainWindow::on_pushButton_1p5x_clicked()
 {
     playbackRateHandler->chnagePlaybackRate(1.5f);
 }


 void MainWindow::on_pushButton_2x_clicked()
 {
     playbackRateHandler->chnagePlaybackRate(2.0f);
 }

 void MainWindow::fullScreenChnaged(const QRectF &rect){
     //view->fitInView(rect);
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
     proxy->setGeometry(QRect(0,ui->centralwidget->height() - 200,ui->centralwidget->width(),200));

     fsPlayPauseButton = controls->findChild<QPushButton*>("fs_PushButton_Play_Pause",Qt::FindChildOption::FindChildrenRecursively);
     fsStopButton = controls->findChild<QPushButton*>("fs_PushButton_Stop",Qt::FindChildOption::FindChildrenRecursively);
     fsSeekbar = controls->findChild<QSlider*>("fs_HorizontalSlider_Duration",Qt::FindChildOption::FindChildrenRecursively);
     fsNormalButton = controls->findChild<QPushButton*>("normal_screen_button",Qt::FindChildOption::FindChildrenRecursively);
     fsTotalTime = controls->findChild<QLabel*>("fs_label_total_time",Qt::FindChildOption::FindChildrenRecursively);
     fsCurrentTime = controls->findChild<QLabel*>("fs_label_current_time",Qt::FindChildOption::FindChildrenRecursively);
     fsTenSecForward = controls->findChild<QPushButton*>("fs_PushButton_Seek_Forward",Qt::FindChildOption::FindChildrenRecursively);
     fsTenSecBackward = controls->findChild<QPushButton*>("fs_PushButton_Seek_Backward",Qt::FindChildOption::FindChildrenRecursively);
     fsSpeed1x = controls->findChild<QPushButton*>("pushButton_1x",Qt::FindChildOption::FindChildrenRecursively);
     fsSpeed1p2x = controls->findChild<QPushButton*>("pushButton_1p2x",Qt::FindChildOption::FindChildrenRecursively);
     fsSpeed1p5x = controls->findChild<QPushButton*>("pushButton_1p5x",Qt::FindChildOption::FindChildrenRecursively);
     fsSpeed2x = controls->findChild<QPushButton*>("pushButton_2x",Qt::FindChildOption::FindChildrenRecursively);
     fsSeekbarVolume = controls->findChild<QSlider*>("fs_HorizontalSlider_Volume",Qt::FindChildOption::FindChildrenRecursively);

     // Connect the buttons to the media player
     /////
     /// \brief connect
     QGraphicsRectItem* hoverArea = new FullScreenControlHoverHandler(proxy, QRectF(0,ui->centralwidget->height() - 200,ui->centralwidget->width(),200));
     hoverArea->setPen(Qt::NoPen);
     hoverArea->setAcceptHoverEvents(true);
     scene->addItem(hoverArea);
     ///
     connect(fsPlayPauseButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_Play_Pause_clicked);
     connect(fsStopButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_Stop_clicked);
     connect(fsNormalButton,&QPushButton::clicked,this, &MainWindow::on_normal_button_pressed);
     ///

     ///////
    fsSeekbarController = new VideoProgressBarController(fsSeekbar,fileCount);
    fsSeekbarController->setSliderMaxLimit(timeLimit);
    fsSeekbarConnection = connect(fsSeekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    fsSeekbarForwardBackwardConnection = connect(fsSeekbarController,&VideoProgressBarController::onSeekbarSecondsTimerEndSliding,this,&MainWindow::seekBackward);
    connect(fsSeekbar,&QSlider::sliderReleased,this,&MainWindow::on_horizontalSlider_Duration_sliderMoved);
    connect(fsSeekbar,&QSlider::actionTriggered,this,&MainWindow::slderClicked);
    connect(fsTenSecForward, &QPushButton::clicked,this,&MainWindow::on_pushButton_Seek_Forward_clicked);
    connect(fsTenSecBackward, &QPushButton::clicked,this,&MainWindow::on_pushButton_Seek_Backward_clicked);

    handlePlayPauseButtonState(Player->playbackState());
    connect(fsSpeed1x,&QPushButton::clicked,this,&MainWindow::on_pushButton_1x_clicked);
    connect(fsSpeed1p2x,&QPushButton::clicked,this,&MainWindow::on_pushButton_1p2x_clicked);
    connect(fsSpeed1p5x,&QPushButton::clicked,this,&MainWindow::on_pushButton_1p5x_clicked);
    connect(fsSpeed2x,&QPushButton::clicked,this,&MainWindow::on_pushButton_2x_clicked);
    onPlaybackRateChanged(Player->playbackRate());
    fsSeekbarVolume->setMaximum(100);
    fsSeekbarVolume->setMinimum(0);
    fsSeekbarVolume->setValue(ui->horizontalSlider_Volume->value());
    connect(fsSeekbarVolume, &QSlider::valueChanged,this,&MainWindow::handleVolumeChange);
    RECORDING_RED_DOT->updatePosition(20, view->height() - 40);
    RECORDING_FLASH_LAYER->updateSize(0,0,view->width(),view->height());
 }


 void MainWindow::on_normal_button_pressed(){
     scene->removeItem(controls->graphicsProxyWidget());
     this->setWindowState(Qt::WindowNoState);
     ui->controlsSection->show();
     // videoItem->setSize(QSize(oldWidth, oldHeight));
     // view->fitInView(videoItem, Qt::KeepAspectRatio);
     // view->setGeometry(0, 0, oldWidth, oldHeight);
     // videoItem->setSize(QSize(oldWidth, oldHeight));
     // view->fitInView(videoItem, Qt::KeepAspectRatio);
     // view->setGeometry(0, 0, oldHeight, oldHeight);
     playbackRateHandler->chnagePlaybackRate(Player->playbackRate());
     ui->horizontalSlider_Volume->setValue(fsSeekbarVolume->value());
     handlePlayPauseButtonState(Player->playbackState());

     videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));
     view->fitInView(videoItem, Qt::KeepAspectRatio);
     view->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
     update();
     //view->setAlignment(Qt::AlignCenter);
     //view->centerOn(view->mapToScene( view->viewport()->rect().center() ));
     QObject::disconnect(fsSeekbarConnection);
     QObject::disconnect(fsPlayPauseButton);
     QObject::disconnect(fsStopButton);
     QObject::disconnect(fsNormalButton);
     QObject::disconnect(fsSeekbar);
     QObject::disconnect(fsSpeed1x);
     QObject::disconnect(fsSpeed1p2x);
     QObject::disconnect(fsSpeed1p5x);
     QObject::disconnect(fsSpeed2x);
     QObject::disconnect(fsSeekbarForwardBackwardConnection);
     RECORDING_RED_DOT->updatePosition(20, view->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,view->width(),view->height());
     update();
 }

 void MainWindow::handlePlayPauseButtonState(QMediaPlayer::PlaybackState playbackState){
     if(IS_FULL_SCREEN){
         if(playbackState == QMediaPlayer::PlaybackState::PlayingState){
            fsPlayPauseButton->setStyleSheet( pauseButtonStyle );
         } else {
            fsPlayPauseButton->setStyleSheet( playButtonStyle );
         }
     }else {
         if(playbackState == QMediaPlayer::PlaybackState::PlayingState){
            ui->pushButton_Play_Pause->setStyleSheet( pauseButtonStyle );
         } else {
            ui->pushButton_Play_Pause->setStyleSheet( playButtonStyle );
         }
     }
 }

 void MainWindow::on_pushButton_full_screen_clicked()
 {
     if(fileCount == 0) return;
     oldHeight = ui->centralwidget->height();
     oldWidth = ui->centralwidget->width();
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
     WATERMARK_TEXT->setPos(100,100);
     setupFullScreenControls();
 }

 void MainWindow::handleUserManualFullScreen(){

     if(fileCount == 0) return;
     oldHeight = ui->centralwidget->height();
     oldWidth = ui->centralwidget->width();
     ui->controlsSection->hide();
     videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     view->fitInView(videoItem, Qt::KeepAspectRatio);
     view->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
     WATERMARK_TEXT->setPos(100,100);
     setupFullScreenControls();
 }

 void MainWindow::handleUserManualMaximized(){
     if(fileCount == 0) return;
     videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     view->fitInView(videoItem, Qt::KeepAspectRatio);
     view->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
     WATERMARK_TEXT->setPos(100,100);
     RECORDING_RED_DOT->updatePosition(20, view->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,view->width(),view->height());
     // this is intentioal dont think it is as redundent
     RECORDING_RED_DOT->updatePosition(20, view->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,view->width(),view->height());
 }

 void MainWindow::handleUserManualUnMaximize(){
     if(fileCount == 0) return;
     videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     view->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());

     videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     view->setGeometry(0, 0, ui->video_section->width(), ui->video_section->width());

     scene->setSceneRect(0,0,ui->video_section->width(),ui->video_section->width());

     /////////
     RECORDING_RED_DOT->updatePosition(20, view->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,view->width(),view->height());

     RECORDING_RED_DOT->updatePosition(20, view->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,view->width(),view->height());
 }

 void MainWindow::handleArrowKey(int key){
     switch (key)
     {
     case Qt::Key_Left:
            on_pushButton_Seek_Backward_clicked();
         break;
     case Qt::Key_Right:
            on_pushButton_Seek_Forward_clicked();
         break;
     default:
        // RECORDING_RED_DOT->updatePosition(20, ui->video_section->height() - 40);
         break;
     }
 }
