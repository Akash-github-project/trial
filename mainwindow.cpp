#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "customuiloader.h"
// #define TEST
// 120633,119002

#define IS_FULL_SCREEN windowState().testFlag(Qt::WindowFullScreen)
//#define IS_FULL_SCREEN false


MainWindow::MainWindow(QString filePath,QString token,QString course_id,QString video_id,QString video_item_id,QString identifier,QString deviceId,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,warningDialog(new WarningDialog(this))
{
    ui->setupUi(this);
    guiApp = QGuiApplication::instance();
    guiInstance = qobject_cast<QGuiApplication*>(guiApp);
    setWindowTitle("Video Player");
    QObject::connect(
        warningDialog,
        &WarningDialog::closed,
        this,
        [this](){
            this->warningDialog->close();
            guiInstance->exit();
    });

    this->token = token;
    this->video_id = video_id;
    this->course_id = course_id;
    this->identifier = identifier;
    this->video_item_id = video_item_id;
    widgetVideo = new VideoWidget();
    Player = new VLCPlayer(this,widgetVideo);

    //QAudioOutput *audioOutput = new QAudioOutput();
    seekbarNewController = new SeekbarProgressController(this);
    ui->pushButton_Play_Pause->setStyleSheet( playButtonStyle );

    ui->horizontalSlider_Volume->setMinimum(0);
    ui->horizontalSlider_Volume->setMaximum(100);
    ui->horizontalSlider_Volume->setValue(50);

    //Player->setAudioOutput(audioOutput);
    //Player->audioOutput()->setVolume(ui->horizontalSlider_Volume->value() / 100.0f);
    Player->setVolume(ui->horizontalSlider_Volume->value());

    //ui->horizontalSlider_Duration->setTabletTracking(false);
    ui->horizontalSlider_Duration->setTracking(false);
    playbackRateHandler = new PlaybackRateHandler(this,Player);
    //
    manager = new ApiManager(identifier,token,deviceId,this);
    connect(manager, &ApiManager::onKeyFetchFinished, this, &MainWindow::onKeyFetchCompleted);
    connect(manager, &ApiManager::noNetwork, this, &MainWindow::onNoInternet);

    //don
    //connect(Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(Player, &VLCPlayer::positionChanged, this, &MainWindow::positionChanged);
    //connect(Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    //no change
    //connect(Player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::loadVideo);
    connect(Player, &VLCPlayer::mediaStatusChanged, this, &MainWindow::loadVideo);


    //connect(Player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::handlePlayPauseButtonState);
    connect(Player, &VLCPlayer::playPausedStatusChanged, this, &MainWindow::handlePlayPauseButtonState);

    seekbarController = new VideoProgressBarController(ui->horizontalSlider_Duration,0);
    seekbarController->setSliderMaxLimit(getSumOfAllVideosTimeTillNow(videoItemList.count() - 1));
    seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    connect(ui->horizontalSlider_Duration,&CustomSeekbar::valueChanged,this,&MainWindow::on_horizontalSlider_Duration_sliderMoved);

    connect(ui->horizontalSlider_Duration,&CustomSeekbar::userClickOnSeekbar,[this](){
        this->userAction = true;
    });
//    connect(ui->horizontalSlider_Duration,&CustomSeekbar::actionTriggered,this,&MainWindow::slderClicked);
    connect(ui->horizontalSlider_Volume,&QSlider::valueChanged,this,&MainWindow::handleVolumeChange);
    connect(playbackRateHandler,&PlaybackRateHandler::playbackRateChanged,this,&MainWindow::onPlaybackRateChanged);

    connect(manager,&ApiManager::noNetworkForTimer,this,&MainWindow::userPlaytimeDataFailed);
    connect(manager,&ApiManager::onUserTimeSentSuccess,this,&MainWindow::userPlaytimeDataSuccess);

    handler = new EncryptionHandler();
    fullScreenEventHandler = new WindowEventHandler(this,this);
    // don't know if we need this class

    setFixedSize(820,604);
    disableScreenRecording();
    this->folderPath = filePath;
    // this->playbackTimer = new UserPlaybackTimerTracker(Player,this);
    // // Connect to the play time updated signal
    // connect(playbackTimer, &UserPlaybackTimerTracker::sendMetrics, this,&MainWindow::sendTimeToServer);

     ui->pushButton_1x->hide();
     ui->pushButton_1p2x->hide();
     ui->pushButton_1p5x->hide();
     ui->pushButton_2x->hide();
    // ui->label_3->hide();
     ui->comboBox->setEditable(false);
     preventSleep(true);
}

void MainWindow::sendTimeToServer(qint64 playTimeInSeconds){
    qint64 currentSeekbarPostion = getCurrentSeekabrPosition();
    this->manager->sendUserWatchTime(token,course_id,video_item_id,video_id,playTimeInSeconds,currentSeekbarPostion);
}

void MainWindow::enumerateDisplays() {
    DISPLAY_DEVICE displayDevice;
    displayDevice.cb = sizeof(DISPLAY_DEVICE);
    int deviceIndex = 0;

    while (EnumDisplayDevices(NULL, deviceIndex, &displayDevice, 0)) {
        //qWarning() << "Device Name: " << displayDevice.DeviceName ;
        //qWarning() << "Device String: " << displayDevice.DeviceString ;


        if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
            qDebug() << "P-D";
        }

        if (displayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) {
            qDebug() << "M-D";
        }

        qDebug() << "---------------------";

        deviceIndex++;
    }
    qDebug()<<"count of monitors" << deviceIndex;
}


void MainWindow::showWarningDialog(){
    auto allMonitors = getAllMonitorSizes();
    ScreenDetector detector = ScreenDetector();
    int monitorCount = detector.getMonitorInfoFromDeviceManager();
    try {
        if((allMonitors.length() > 1) || (monitorCount > 1) && warningDialog != nullptr){
            if(Player != nullptr && Player->isPlaying() && IS_FULL_SCREEN){
                on_normal_button_pressed();
                onStopClicked();
                blockedForPiracy = true;
            }else {
                blockedForPiracy = true;
            }
            warningDialog->show();
        }
    }catch (const std::exception &e) {
        // Catch standard exceptions derived from std::exception
        qDebug() << "Standard exception caught:" << e.what();
    }
}


void MainWindow::preventSleep(bool enable) {
    if (enable) {
        SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
    } else {
        SetThreadExecutionState(ES_CONTINUOUS);
    }
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
    connect(RECORDING_FLASH_LAYER, &ScreenFlashLayer::doubleClickedScreen,this,&MainWindow::on_pushButton_full_screen_clicked);
    QShortcut *playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(playPauseShortcut, &QShortcut::activated, this, &MainWindow::on_pushButton_Play_Pause_clicked);
    QShortcut *normalWindowShortcut = new QShortcut(QKeySequence(Qt::Key_Escape),this);
    connect(normalWindowShortcut, &QShortcut::activated, this, &MainWindow::on_normal_button_pressed);

}

void MainWindow::handleWindowModesTransitions(bool isFullScreen){
    //qDebug()<<"full screen";
    //setupFullScreenControls();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Space){
       on_pushButton_Play_Pause_clicked();
    }
    if (event->key() == Qt::Key_Escape && IS_FULL_SCREEN) {
        // Handle Esc key press here
        // Example: Close fullscreen, exit modal, or perform other actions
         on_normal_button_pressed();  // Exit fullscreen if the window is in fullscreen mode
    }
    else {
        // Pass the event to the base class for default processing
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::seekToRemainingTime(long oldTime){
    qDebug()<<"-------------------------";
    qDebug()<<"seekToRemainingTime";
    jumpToPosition(oldTime);
    if(IS_FULL_SCREEN){
        fsSeekbarController->markSeekPending(false);
    }else {
        seekbarController->markSeekPending(false);
    }
}



void MainWindow::slderClicked(int action){
    //qDebug()<<"slider clicked called"<<action<<"slider value"<<seekbarController->getValue();
    // if(IS_FULL_SCREEN){
    //     fsSeekbarController->jumpInstantly(this);
    // }else {
    //     seekbarController->jumpInstantly(this);
    // }
}

void MainWindow::handleVolumeChange(int volume){
    Player->setVolume(volume);
    //Player->audioOutput()->setVolume(volume / 100.0f);
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
    }
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


MainWindow::~MainWindow()
{
    preventSleep(false);
    delete ui;
}

void MainWindow::durationChanged(qint64 duration)
{
    qDebug()<<"-------------------------";
    qDebug()<<"durationChanged called";
    if(IS_FULL_SCREEN && fsSeekbar != nullptr){
        fsSeekbarController->setSliderMaxLimit(this->fullVideoDuration / 1000);
    }else {
        //seekbarController->setSliderMaxLimit(getSumOfAllVideosTimeTillNow(videoItemList.count() - 1));
        seekbarController->setSliderMaxLimit(this->fullVideoDuration / 1000 );
    }
}

void MainWindow::positionChanged(qint64 duration)
{
    // qDebug()<<"-------------------------";
    // qDebug()<<"positionChanged called";
    //qDebug()<<currentIndex << "current index -- duration";
    //qDebug()<<videoTimeArray << "video time array -- duration";
    int sum = 0;
    for(int i= 0;i<currentIndex;i++){
        sum += videoTimeArray[i];
    }
    //qint64 moveTo = (sum / 1000) + (duration / 1000) ;
    //qDebug()<<"sum" << sum;
    qint64 moveTo = (sum / 1000) + duration;
    if(IS_FULL_SCREEN && fsSeekbar != nullptr){
        if(!fsSeekbar->isSliderDown() && !fsSeekbarController->seekPending) {
            fsSeekbarController->moveSlider(moveTo);
        }
    }else {
        if (!ui->horizontalSlider_Duration->isSliderDown() && !seekbarController->seekPending)
        {
            seekbarController->moveSlider(moveTo);
        }
    }
    updateDuration(moveTo);
    if((QDateTime::currentSecsSinceEpoch() - lastDisplayTime > 1)){
        lastDisplayTime = QDateTime::currentSecsSinceEpoch();
        //WATERMARK_TEXT->setPos(x,y);
        ///TODO: come back here
        watermarkHandler->updateWatermark(ui->video_section->width(),ui->video_section->height());
        // scene->update();
    }
}

void MainWindow::onlyUpdatePlaybackTimeText(QString playbackDurationString){

    qint64 fullVideoDurtaion = 0;
    for(int i = 0;i< videoTimeArray.length();i++){
        fullVideoDurtaion += videoTimeArray[i] / 1000;
    }

    QTime TotalTime((fullVideoDurtaion / 3600) % 60, (fullVideoDurtaion / 60) % 60, fullVideoDurtaion % 60, (fullVideoDurtaion * 1000) % 1000);
    QString Format ="";
    if (fullVideoDurtaion > 3600) Format = "hh:mm:ss";
    else Format = "mm:ss";
    if(IS_FULL_SCREEN && fsSeekbar != nullptr && fsTotalTime != nullptr && fsCurrentTime != nullptr){
        fsTotalTime->setText(TotalTime.toString(Format));
        ui->label_Total_Time->setText(TotalTime.toString(Format));
        fsCurrentTime->setText(playbackDurationString);
        ui->label_current_Time->setText(playbackDurationString);
    }else {
        ui->label_current_Time->setText(playbackDurationString);
        ui->label_Total_Time->setText(TotalTime.toString(Format));
    }
}

void MainWindow::updateDuration(qint64 Duration)
{
    qint64 fullVideoDurationLocal = 0;
    for(int i = 0;i< videoTimeArray.length();i++){
        fullVideoDurationLocal += videoTimeArray[i] / 1000;
    }

    if (Duration || fullVideoDurationLocal)
    {
        qint64 videoDurationInSeconds = this->fullVideoDuration / 1000;
        QTime CurrentTime((Duration / 3600) % 60, (Duration / 60) % 60, Duration % 60, (Duration * 1000) % 1000);
        QTime TotalTime((videoDurationInSeconds / 3600) % 60, (videoDurationInSeconds / 60) % 60, videoDurationInSeconds % 60, (videoDurationInSeconds * 1000) % 1000);
        QString Format ="";
        if (videoDurationInSeconds > 3600) Format = "hh:mm:ss";
        else Format = "mm:ss";

        //qDebug()<<"Duration "<<Duration;
        //qDebug()<<videoDurationInSeconds << "video duration in seconds";
        //qDebug()<<CurrentTime << "Current Time";
        //qDebug()<<TotalTime << "Tota Time";

        // qDebug()<<"time ------ " << TotalTime.toString(Format);
        if(IS_FULL_SCREEN && fsSeekbar != nullptr && fsTotalTime != nullptr && fsCurrentTime != nullptr){
            fsTotalTime->setText(TotalTime.toString(Format));
            fsCurrentTime->setText(CurrentTime.toString(Format));
            // ui->label_current_Time->setText(CurrentTime.toString(Format));
            // ui->label_Total_Time->setText(TotalTime.toString(Format));
        }else {
            ui->label_current_Time->setText(CurrentTime.toString(Format));
            ui->label_Total_Time->setText(TotalTime.toString(Format));
        }
    }
}

void MainWindow::on_actionOpen_triggered(MizuConfig * config)
{
    this->intervalForWm = config->smConfig->interval;
    selectedDirectory = this->folderPath;
    QDir directory(selectedDirectory);
    QString filter = videoFileChunkPattern;
    QStringList files = directory.entryList(QStringList() << filter,QDir::Files);
    fileCount = files.length();
    if(fileCount == 0){
        return;
    }

    /////
    // // Create a QGraphicsScene
    // scene = new PlayerControllerWidget();
    // view = new CustomGraphicsView();
    // // scene->addWidget(widgetVideo);


    // view->installEventFilter(this);
    // view->setScene(scene);

    // view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // // Create a QGraphicsVideoItem
    // videoItem = new QGraphicsVideoItem();
    //scene->addItem(videoItem);

    wMarkScreen = new CustomGestureWidget(this);

    watermarkHandler = new MizuShirushiHandora(this,wMarkScreen,this->identifier,3,3,config);

    // WATERMARK_TEXT = new QGraphicsTextItem("");
    // WATERMARK_TEXT1 = new QGraphicsTextItem("");
    // QFont font("Arial", 24, QFont::Bold);
    // WATERMARK_TEXT->setFont(font);
    // WATERMARK_TEXT->setDefaultTextColor(Qt::red);
    // WATERMARK_TEXT->setOpacity(0.5f);
    // WATERMARK_TEXT->setPos(10, 10); // Position the watermark
    // //
    // WATERMARK_TEXT1->setFont(font);
    // WATERMARK_TEXT1->setDefaultTextColor(Qt::red);
    // WATERMARK_TEXT1->setOpacity(0.5f);
    // WATERMARK_TEXT1->setPos(10, 10); // Position the watermark

    watermarkHandler->postionPrimary(ui->centralwidget->width(),ui->centralwidget->height());
    // red recording dot
    // scene->addItem(WATERMARK_TEXT);
    // scene->addItem(WATERMARK_TEXT1);

    RECORDING_RED_DOT = new RedDotRecording(wMarkScreen,this->identifier.toStdString(),config->dflConfig,ui->video_section);
    RECORDING_FLASH_LAYER = new ScreenFlashLayer(wMarkScreen,phoneNumber, config->flConfig, ui->video_section);
    //Player->setVideoOutput(videoItem);
    RECORDING_RED_DOT->updatePosition(20, wMarkScreen->height());
    RECORDING_FLASH_LAYER->updateSize(0,0,ui->centralwidget->width(),ui->centralwidget->height());

    ///
    /// \brief file
    ///
    QFile file(directory.filePath(files.first()));
    file.open(QIODevice::ReadOnly);
    QByteArray videoArray = file.readAll();
    openParticularChunk(videoArray,0);
    //ui->video_section->setRenderHint(QPainter::SmoothPixmapTransform);
    //ui->video_section->setBackgroundBrush(Qt::black);
    //ui->video_section->setFrameShape(QFrame::NoFrame);
    //ui->video_section->fitInView(videoItem, Qt::KeepAspectRatio);
    //ui->video_section->setParent(ui->video_section);

    ui->video_section->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
    //videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));

    ui->video_section->show();
    seekbarNewController->startTimer();

    QObject::disconnect(seekbarConnection);
    seekbarController = new VideoProgressBarController(ui->horizontalSlider_Duration,fileCount);
    connect(seekbarController,&VideoProgressBarController::onSeekbarSecondsTimerEndSliding,this,&MainWindow::seekToRemainingTime);
    seekbarController->setSliderMaxLimit(getSumOfAllVideosTimeTillNow(videoItemList.count() - 1));
    seekbarConnection = connect(seekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
    playbackRateHandler->chnagePlaybackRate(1.0f);
    RECORDING_FLASH_LAYER->startFlasing();
    RECORDING_RED_DOT->startFlasing();
    RECORDING_RED_DOT->updatePosition(20, ui->video_section->height() - 40);

    //videoView = new QWidget(ui->video_section);
    //ui->video_section->


    videoStackedLayout = new QStackedLayout(this);
    videoStackedLayout->setStackingMode(QStackedLayout::StackAll);
    videoStackedLayout->addWidget(widgetVideo);


    CustomUILoader loader;
    QFile uiFile(":/controls_overlay.ui"); // Ensure the path to your .ui file is correct
    uiFile.open(QFile::ReadOnly);
    controls = loader.load(&uiFile);
    uiFile.close();

    QVBoxLayout* controlsLayout =  new QVBoxLayout(this);

    controlsLayout->addWidget(controls);
    controlsLayout->setAlignment(controls,Qt::AlignBottom | Qt::AlignHCenter);

    controlsWrapper = new FullScreenControlsWidget(this,controls);
    controlsWrapper->setStyleSheet("background-color:transparent;");
    controlsWrapper->setLayout(controlsLayout);
    wMarkScreen->setStyleSheet("background-color:transparent;");
    videoStackedLayout->addWidget(wMarkScreen);
    videoStackedLayout->addWidget(controlsWrapper);


    ui->video_section->setLayout(videoStackedLayout);
    controlsWrapper->hide();

    connect(controlsWrapper,&FullScreenControlsWidget::exitFullScreen,[this](){
        this->on_normal_button_pressed();
    });

    connect(wMarkScreen,&CustomGestureWidget::makeFullScreen,[this](){
        this->on_pushButton_full_screen_clicked();
    });
    //controls->hide();

}


void MainWindow::on_pushButton_Play_Pause_clicked(){
    qDebug()<<"-------------------------";
    qDebug()<<"on_pushButton_Play_Pause_clicked called";
    qDebug()<<"Playback state " << Player->playbackState();
    if(pausedForNoInternet) return;
    if(blockedForPiracy) return;
    if(fileCount <= 0) return;

    if(Player->playbackState() == PlaybackState::Playing){
        seekbarNewController->pause();
        qDebug()<<"jumping to ###############" << seekbarController->getValue();
        Player->pause();
        isPaused = true;
    }else {
        if(Player->playbackState() == PlaybackState::Playing && mediaStopped == true){
            Player->pause();
        }
        seekbarNewController->resume();
        Player->resume();

        if(IS_FULL_SCREEN){
           int videoIndexToJump = getVideoIndexToJump(fsSeekbarController->getValue());
            currentIndex = videoIndexToJump;
           qDebug()<<"jumping to ***********" << fsSeekbarController->getValue();
        }else {
           int videoIndexToJump = getVideoIndexToJump(seekbarController->getValue());
            currentIndex = videoIndexToJump;
            qDebug()<<"jumping to **********" << seekbarController->getValue();
        }
        mediaStopped = false;
        isPaused = false;
    }
}

void MainWindow::onStopClicked(){
    if(pausedForNoInternet){
        return;
    }
    if(IS_FULL_SCREEN){
        fsSeekbarController->moveSlider(0);
        seekbarController->moveSlider(0);
    } else {
        seekbarController->moveSlider(0);
    }
    currentIndex = -1;
    mediaStopped = true;
    jumpToPosition(0);
    if(Player != nullptr){
        Player->pause();
    }
    handlePlayPauseButtonState(PlayPauseState::Pause);
}

void MainWindow::on_pushButton_Stop_clicked()
{
    if(IS_FULL_SCREEN){
      on_normal_button_pressed();
    }
    if(pausedForNoInternet){
        return;
    }
    if(IS_FULL_SCREEN){
        fsSeekbarController->moveSlider(0);
        seekbarController->moveSlider(0);
    } else {
        seekbarController->moveSlider(0);
    }
    //sliderTime = -1;
    currentIndex = -1;
    //IS_Pause = true;
    mediaStopped = true;
    jumpToPosition(0);
    positionChanged(0);
    if(Player != nullptr){
        Player->pause();
    }
    handlePlayPauseButtonState(PlayPauseState::Pause);
}

void MainWindow::on_pushButton_Volume_clicked()
{
    if (IS_Muted == false)
    {
        IS_Muted = true;
        Player->setMuted(true);
        //Player->audioOutput()->setMuted(true);
    }
    else
    {
        IS_Muted = false;
        Player->setMuted(false);
        //Player->audioOutput()->setMuted(false);
    }
}

void MainWindow::on_horizontalSlider_Volume_valueChanged(int value)
{
    //if(Player != NULL && Player->audioOutput() != NULL){
    if(Player != NULL){
        Player->setVolume(value);
        //Player->audioOutput()->setVolume(value / 100.0f);
        if(value == 0){
            changeVolumeIconToMute();
        }else {
            changeVolumeIconToLowFromMute();
        }
    }
}

void MainWindow::changeVolumeIconToMute(){
    //ui->label_low_volume->
    // this is temporary needs to be changed with new mute icon
    if(IS_FULL_SCREEN){
        // fsVolumeLowIcon->setStyleSheet(muteStyle);
        fsVolumeLowIcon->setPixmap(QPixmap(":/mute_volume"));
        update();
    }else {
        ui->label_low_volume->setPixmap( QPixmap(":/mute_volume"));
        //ui->label_low_volume->setStyleSheet(muteStyle);
    }
}

void MainWindow::changeVolumeIconToLowFromMute(){
    if(IS_FULL_SCREEN){
        fsVolumeLowIcon->setPixmap(QPixmap(":/volume-low"));
        //fsVolumeLowIcon->setStyleSheet(lowVolumeStyle);
        update();
    }else {
        ui->label_low_volume->setPixmap(QPixmap(":/volume-low"));
        //ui->label_low_volume->setStyleSheet(lowVolumeStyle);
    }
}


void MainWindow::on_pushButton_Seek_Backward_clicked() {
    qDebug()<<"-------------------------";
    qDebug()<<"on_pushButton_Seek_Backward_clicked called";
    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if((currentTime - lastBackward) < 100){
        return;
    }

    if(pausedForNoInternet) return;
    if(blockedForPiracy) return;
    if(fileCount <= 0) return;
    if(IS_FULL_SCREEN){
        int oldTime = fsSeekbarController->getValue() - 10;
        userAction = true;
        if(oldTime <= fsSeekbar->minimum()){
            onStopClicked();
            return;
        }
            fsSeekbarController->moveSlider(fsSeekbarController->getValue() - 10);
    }else {
        int oldTime = seekbarController->getValue() - 10;
        userAction = true;
        if(oldTime <= ui->horizontalSlider_Duration->minimum()){
            onStopClicked();
            return;
        }
        seekbarController->moveSlider(seekbarController->getValue() - 10);
    }

    lastBackward = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

int MainWindow::getSumOfAllVideosTimeTillNow(int index){
    qint64 fullVideoDurtaion = 0;
    for(int i = 0;(i< videoTimeArray.length()) && (index < videoTimeArray.size()) && (index >= 0);i++){
        fullVideoDurtaion += videoTimeArray[i] / 1000;
    }
    return fullVideoDurtaion;
}


void MainWindow::on_pushButton_Seek_Forward_clicked()
{
    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if((currentTime - lastForward) < 100){
        return;
    }
    qDebug()<<"-------------------------";
    qDebug()<<"on_pushButton_Seek_Forward_clicked called";
    if(pausedForNoInternet) return;
    if(blockedForPiracy) return;
    if(fileCount <= 0) return;
    if(IS_FULL_SCREEN){
        int oldTime = fsSeekbarController->getValue() + 10;
        if(oldTime >= fsSeekbar->maximum()){
            onStopClicked();
            return;
        }

        userAction = true;
        fsSeekbarController->moveSlider(fsSeekbarController->getValue() + 10);
    }else {
        int oldTime = seekbarController->getValue() + 10;
        if(oldTime >= ui->horizontalSlider_Duration->maximum()){
            onStopClicked();
            return;
        }
        userAction = true;
        seekbarController->moveSlider(seekbarController->getValue() + 10);
    }
    lastForward = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

//void MainWindow::loadVideo(QMediaPlayer::MediaStatus status){
void MainWindow::loadVideo(PlaybackState status){
    //qDebug()<<"media status"<<status;

    if(status == PlaybackState::Changed && !isPaused && mediaStopped == false && !blockedForPiracy && !pausedForNoInternet){
        //Player->play();
        qDebug()<<"media changed";
        caliberateVideo();
    }

    // if(status == QMediaPlayer::MediaStatus::LoadedMedia && !isPaused && mediaStopped == false && !blockedForPiracy && !pausedForNoInternet){
    //     Player->play();
    //     caliberateVideo();
    // }

     //if(status != QMediaPlayer::MediaStatus::EndOfMedia){
    if(status != PlaybackState::Ended){
        return;
    } else {
        qDebug()<<"media ended";
        currentIndex++;
        seekbarNewController->pause();
        qDebug()<<"current index -- loadVideo" << currentIndex;
    }

    if(fileCount <= currentIndex) {
        currentIndex = 0;
        onStopClicked();
        playbackTimer->emitMetricsSignal();
        updateDuration(0);
        return;
    }

    QDir directory(selectedDirectory);
    QStringList filesToPlay = getFileList(selectedDirectory);
    QFile file(directory.filePath(filesToPlay[currentIndex]));
    if (!file.open(QIODevice::ReadOnly)) {
//        qDebug() << "Failed to open file for reading:" << file.errorString();
        return; // or handle the error in some way
    }
//    qDebug()<<"current index to play" << currentIndex;
    QByteArray videoArray = file.readAll();
    file.close(); // Close the file after reading
    openParticularChunk(videoArray,currentIndex);
    seekbarNewController->resume();
    //Player->play();

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
    qDebug()<<"-------------------------";
    qDebug()<<"onSliderStop called";
    seekbarNewController->markDraggingEnded();
    int position = 0;
    if(IS_FULL_SCREEN){
        position = fsSeekbarController->getValue();
    }else {
        position = seekbarController->getValue();
    }

    jumpToPosition(position);

 }

 void MainWindow::on_horizontalSlider_Duration_sliderMoved(){
    // qDebug()<<"-------------------------";
    // qDebug()<<"on_horizontalSlider_Duration_sliderMoved called";

    if(IS_FULL_SCREEN){
        if( fsSeekbarController->horizontalSlider_Duration->isSliderDown() || userAction ){
            fsSeekbarController->setupTimer(this);
            seekbarNewController->markGettingDragged();
        }
     }else {
        if(seekbarController->horizontalSlider_Duration->isSliderDown() || userAction){
            seekbarController->setupTimer(this);
            seekbarNewController->markGettingDragged();
        }
     }
 }


 void MainWindow::jumpToPosition(int secondToJump){
    qDebug()<<"-------------------------";
    qDebug()<<"jumpToPosition called";
     if(blockedForPiracy) return;
     int videoIndexToJump = getVideoIndexToJump(secondToJump);
     int extraSecondsSeek = getExtraSeek(secondToJump,videoIndexToJump);
     loadParticalarChunk(videoIndexToJump,extraSecondsSeek);
 }

 int MainWindow::getExtraSeek(int timeInSeconds,int indexToJump){
     int sumToIndex = 0;
     int timeSeconds = 1000 * timeInSeconds;
     for(int i = 0; i< indexToJump;i++){
         sumToIndex += videoTimeArray[i];
     }

     if((sumToIndex / 1000) >= seekbarController->horizontalSlider_Duration->maximum()){
         return videoTimeArray[videoTimeArray.size() - 1];
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

     for (int i = 0;i< videoTimeArray.length(); i++){
         difference -= videoTimeArray[i];
         if(difference < 0){
             indexToReturn = i;
             break;
         }
         if(difference == 0 && (i == (videoTimeArray.length() - 1))){
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

    qDebug()<<"-------------------------";
    qDebug()<<"loadParticalarChunk called";
    qDebug()<<" Jumping to video index" << videoIndex;
    qDebug()<<" Jumping to extraSeek" << extraSeek;
     if(blockedForPiracy) return;
     if(fileCount <= videoIndex) {
         seekbarNewController->pause();
         return;
     }

    if(currentIndex == videoIndex){
         if(userAction == true){
             userAction = false;
         }
         Player->movePointerToPosition(extraSeek);
        return;
    }else {
        currentIndex = videoIndex;
    }

    //qWarning()<<videoIndex;
    QDir directory(selectedDirectory);
    QStringList filesToPlay = getFileList(selectedDirectory);
    if(filesToPlay.length() > videoIndex){
        QString fileName = filesToPlay[videoIndex];
        QFile file(directory.filePath(fileName));
        if (!file.open(QIODevice::ReadOnly)) {
            //qDebug() << "Failed to open file for reading:" << file.errorString();
            return; // or handle the error in some way
        }

        //2:33:41
        if(IS_FULL_SCREEN){
            fsSeekbarController->markSeekPending(true);
        }else {
            seekbarController->markSeekPending(true);
        }

        QByteArray videoArray = file.readAll();
        file.close(); // Close the file after reading
        openParticularChunk(videoArray,videoIndex);
        if(IS_FULL_SCREEN){
            fsSeekbarController->setupSeekTimer(this,fsSeekbarController->getValue());
        }else {
            seekbarController->setupSeekTimer(this,seekbarController->getValue());
        }
        if(userAction == true){
             userAction = false;
        }
    }
 }


 void MainWindow::openParticularChunk(QByteArray byteData,int videoIndex){
    // Create a new buffer and set its data
    qDebug()<<"-------------------------";
    qDebug()<<"openParticularChunk called";
    QBuffer* newBuffer = new QBuffer();
    //qDebug() << "vid index" << videoIndex;
    //qDebug() << "byte data" << byteData.length();

    newBuffer->setData(handler->decryptFile(byteData,videoItemList[videoIndex]));
    //qDebug() << " setting data";

    // Open the buffer for reading
    if (!newBuffer->open(QIODevice::ReadOnly)) {
        //qDebug() << "Failed to open buffer for reading";
        return; // or handle the error in some waa
    }
    //Player->setSourceDevice(newBuffer, QUrl("someelse.mp4"));
    Player->setMediaSource(newBuffer);
    Player->play();
    //Player->setSourceDevice();
    //qDebug() << "after here";
 }

 void MainWindow::resizeEvent(QResizeEvent* event)
 {
     QMainWindow::resizeEvent(event);
 }

 void MainWindow::caliberateVideo(){
     if(currentIndex >= 0 && currentIndex < videoItemList.size()){
         videoTimeArray[currentIndex] = Player->duration() * 1000;
     }
 }


 void MainWindow::on_pushButton_1x_clicked()
 {
     Player->changeSpeed(1.0f);
     playbackRateHandler->chnagePlaybackRate(1.0f);
 }


 void MainWindow::on_pushButton_1p2x_clicked()
 {
     Player->changeSpeed(1.2f);
     playbackRateHandler->chnagePlaybackRate(1.2f);
 }


 void MainWindow::on_pushButton_1p5x_clicked()
 {
     Player->changeSpeed(1.5f);
    playbackRateHandler->chnagePlaybackRate(1.5f);
 }


 void MainWindow::on_pushButton_2x_clicked()
 {
     Player->changeSpeed(2.0f);
    playbackRateHandler->chnagePlaybackRate(2.0f);
 }

 void MainWindow::fullScreenChnaged(const QRectF &rect){
     //view->fitInView(rect);
 }


 void MainWindow::setupFullScreenControls(){
     try {
         // Find the controls in the loaded UI
         // Add the UI as a proxy widget to the scene

         controlsWrapper->show();
         controls->setGeometry(QRect(0,ui->centralwidget->height() - 200,ui->centralwidget->width(),200));


         if(fsPlayPauseButton == nullptr){
            fsPlayPauseButton = controls->findChild<QPushButton*>("fs_PushButton_Play_Pause",Qt::FindChildOption::FindChildrenRecursively);
            connect(fsPlayPauseButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_Play_Pause_clicked);
         }
         if(fsStopButton == nullptr){
            fsStopButton = controls->findChild<QPushButton*>("fs_PushButton_Stop",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsSeekbar == nullptr){
            fsSeekbar = qobject_cast<CustomSeekbar*>(controls->findChild<QSlider*>("fs_HorizontalSlider_Duration",Qt::FindChildOption::FindChildrenRecursively));
         }
         if(fsNormalButton == nullptr){
            fsNormalButton = controls->findChild<QPushButton*>("normal_screen_button",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsTotalTime == nullptr){
            fsTotalTime = controls->findChild<QLabel*>("fs_label_total_time",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsCurrentTime == nullptr){
            fsCurrentTime = controls->findChild<QLabel*>("fs_label_current_time",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsTenSecBackward == nullptr){
            fsTenSecForward = controls->findChild<QPushButton*>("fs_PushButton_Seek_Forward",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsTenSecBackward == nullptr){
            fsTenSecBackward = controls->findChild<QPushButton*>("fs_PushButton_Seek_Backward",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsSpeed1x == nullptr){
            fsSpeed1x = controls->findChild<QPushButton*>("pushButton_1x",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsSpeed1p2x == nullptr){
            fsSpeed1p2x = controls->findChild<QPushButton*>("pushButton_1p2x",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsSpeed1p5x == nullptr){
            fsSpeed1p5x = controls->findChild<QPushButton*>("pushButton_1p5x",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsSpeed2x == nullptr){
            fsSpeed2x = controls->findChild<QPushButton*>("pushButton_2x",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsSpeedBox == nullptr){
            fsSpeedBox = controls->findChild<QComboBox*>("comboBoxSpeed",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsSeekbarVolume == nullptr){
            fsSeekbarVolume = controls->findChild<QSlider*>("fs_HorizontalSlider_Volume",Qt::FindChildOption::FindChildrenRecursively);
         }
         // Connect the buttons to the media player
         if(fsVolumeLowIcon == nullptr){
            fsVolumeLowIcon = controls->findChild<QLabel*>("label_2",Qt::FindChildOption::FindChildrenRecursively);
         }
         if(fsPlaybackLabel == nullptr){
            fsPlaybackLabel = controls->findChild<QLabel*>("label_3",Qt::FindChildOption::FindChildrenRecursively);
         }

         fsSpeed2x->hide();
         fsSpeed1p2x->hide();
         fsSpeed1p5x->hide();
         fsSpeed1x->hide();

         /////
         /// \brief connect
         // FullScreenControlHoverHandler* hoverArea = new FullScreenControlHoverHandler(proxy, QRectF(0,ui->centralwidget->height() - 200,ui->centralwidget->width(),200));
         // hoverArea->setPen(Qt::NoPen);
         // hoverArea->setAcceptHoverEvents(true);
         // hoverArea->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
         // scene->addItem(hoverArea);

         //connect(fsPlayPauseButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_Play_Pause_clicked);
         connect(fsStopButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_Stop_clicked);
         connect(fsNormalButton,&QPushButton::clicked,this, &MainWindow::on_normal_button_pressed);
         ///

         ///////
        fsSeekbarController = new VideoProgressBarController(fsSeekbar,fileCount);
        fsSeekbarController->setSliderMaxLimit(getSumOfAllVideosTimeTillNow(videoItemList.count() - 1));
        fsSeekbar->setTracking(false);
        fsSeekbarConnection = connect(fsSeekbarController,&VideoProgressBarController::onSeekbarStopedSliding,this,&MainWindow::onSliderStop);
        fsSeekbarForwardBackwardConnection = connect(fsSeekbarController,&VideoProgressBarController::onSeekbarSecondsTimerEndSliding,this,&MainWindow::seekToRemainingTime);
        connect(fsSeekbar,&CustomSeekbar::valueChanged,this,&MainWindow::on_horizontalSlider_Duration_sliderMoved);
        //connect(fsSeekbar,&CustomSeekbar::actionTriggered,this,&MainWindow::slderClicked);
        connect(fsTenSecForward, &QPushButton::clicked,this,&MainWindow::on_pushButton_Seek_Forward_clicked);
        connect(fsTenSecBackward, &QPushButton::clicked,this,&MainWindow::on_pushButton_Seek_Backward_clicked);
        connect(fsSeekbar,&CustomSeekbar::userClickOnSeekbar,[this](){
            this->userAction = true;
        });
        handlePlayPauseButtonState(Player->playPauseState());
        connect(fsSpeed1x,&QPushButton::clicked,this,&MainWindow::on_pushButton_1x_clicked);

        connect(fsSpeed1p2x,&QPushButton::clicked,this,&MainWindow::on_pushButton_1p2x_clicked);
        connect(fsSpeed1p5x,&QPushButton::clicked,this,&MainWindow::on_pushButton_1p5x_clicked);
        connect(fsSpeed2x,&QPushButton::clicked,this,&MainWindow::on_pushButton_2x_clicked);
        connect(fsSpeedBox, &QComboBox::currentTextChanged, this, &MainWindow::on_comboBox_currentTextChanged);

        onPlaybackRateChanged(Player->playbackRate());
        fsSeekbarVolume->setMaximum(100);
        fsSeekbarVolume->setMinimum(0);
        fsSeekbarVolume->setValue(ui->horizontalSlider_Volume->value());
        fsSeekbarController->moveSlider(seekbarController->getValue());
        connect(fsSeekbarVolume, &QSlider::valueChanged,this,&MainWindow::on_horizontalSlider_Volume_valueChanged);
        RECORDING_RED_DOT->updatePosition(20, ui->video_section->height() - 40);
        RECORDING_FLASH_LAYER->updateSize(0,0,ui->video_section->width(),ui->video_section->height());
        onlyUpdatePlaybackTimeText(ui->label_current_Time->text());

        //
        caliberateVideo();
        if(fsSeekbarVolume->value() == 0){
            changeVolumeIconToMute();
        }
        if(pausedForNoInternet){
            fsSeekbar->setDisabled(true);
        }
        this->fsSpeedBox->setCurrentText(ui->comboBox->currentText());
        this->fsSpeedBox->setEditable(false);
     }catch(std::exception e){
         qDebug()<<e.what() << "exception";
     }
 }


 void MainWindow::on_normal_button_pressed(){
     try {
         if(!IS_FULL_SCREEN) return;
         QString speedText = this->fsSpeedBox->currentText();
         this->fsSpeedBox->unsetCursor();
         //controls->hide();
         controlsWrapper->hide();
         int currentSeekValue = fsSeekbarController->getValue();
         QString currentTimeLabelValue = fsCurrentTime->text();
         //scene->removeItem(controls->graphicsProxyWidget());
         this->setWindowState(Qt::WindowNoState);
         ui->controlsSection->show();
         //playbackRateHandler->chnagePlaybackRate(Player->playbackRate());
         ui->horizontalSlider_Volume->setValue(fsSeekbarVolume->value());
         handlePlayPauseButtonState(Player->playPauseState());



         //videoItem->setSize(QSize(ui->video_section->width(), ui->video_section->height()));
         //ui->video_section->fit
         // ui->video_section->fitInView(videoItem, Qt::KeepAspectRatio);
         // ui->video_section->setGeometry(0, 0, ui->video_section->width(), ui->video_section->height());
         update();
         // ui->video_section->setAlignment(Qt::AlignCenter);
         //ui->video_section->centerOn(view->mapToScene( view->viewport()->rect().center() ));
         QObject::disconnect(fsSeekbarConnection);
         QObject::disconnect(fsPlayPauseButton);
         QObject::disconnect(fsStopButton);
         QObject::disconnect(fsNormalButton);
         QObject::disconnect(fsSeekbar);
         QObject::disconnect(fsSpeed1x);
         QObject::disconnect(fsSpeed1p2x);
         QObject::disconnect(fsSpeed1p5x);
         QObject::disconnect(fsSpeed2x);
         QObject::disconnect(fsSpeedBox);
         QObject::disconnect(fsSeekbarForwardBackwardConnection);

         RECORDING_RED_DOT->updatePosition(20, ui->video_section->height() - 40);
         RECORDING_FLASH_LAYER->updateSize(0,0,ui->video_section->width(),ui->video_section->height());
         caliberateVideo();
         if(pausedForNoInternet){
         ui->horizontalSlider_Duration->setDisabled(true);
         }
         seekbarController->moveSlider(currentSeekValue);
         onlyUpdatePlaybackTimeText(currentTimeLabelValue);
         update();
         ui->video_section->update();
         ui->comboBox->setCurrentText(speedText);
         //videoItem->update();
     }catch (std::exception &ex){
         qDebug()<<"exception !!!!!!!!!!!!!!!!" <<ex.what();
     }
 }

 //void MainWindow::handlePlayPauseButtonState(QMediaPlayer::PlaybackState playbackState){
 void MainWindow::handlePlayPauseButtonState(PlayPauseState playbackState){
     //qWarning()<<playbackState;
     if(IS_FULL_SCREEN){
         if(playbackState == PlayPauseState::Play){
            fsPlayPauseButton->setStyleSheet( pauseButtonStyle );
         } else {
            fsPlayPauseButton->setStyleSheet( playButtonStyle );
         }
     }else {
         if(playbackState == PlayPauseState::Play){
            ui->pushButton_Play_Pause->setStyleSheet( pauseButtonStyle );
         } else {
            ui->pushButton_Play_Pause->setStyleSheet( playButtonStyle );
         }
     }
 }

 void MainWindow::on_pushButton_full_screen_clicked()
 {
     if(IS_FULL_SCREEN) return;
     if(fileCount == 0) return;
     oldHeight = ui->centralwidget->height();
     oldWidth = ui->centralwidget->width();
     this->setWindowState(Qt::WindowMaximized);
     ui->controlsSection->hide();
     //videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     //view->fitInView(videoItem, Qt::KeepAspectRatio);
     //view->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
     this->setWindowState(Qt::WindowFullScreen);
     //view->showFullScreen();
     //videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     //view->fitInView(videoItem, Qt::KeepAspectRatio);
     //view->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
     //WATERMARK_TEXT->setPos(100,100);
     //WATERMARK_TEXT->setPos(80,80);
//     watermarkHandler->updateWatermark(ui->centralwidget->width(),ui->centralwidget->height());
     setupFullScreenControls();
 }

 void MainWindow::handleUserManualFullScreen(){

     if(fileCount == 0) return;
     oldHeight = ui->centralwidget->height();
     oldWidth = ui->centralwidget->width();
     ui->controlsSection->hide();
     //videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     //ui->video_section->fitInView(videoItem, Qt::KeepAspectRatio);
     //ui->video_section->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
     watermarkHandler->updateWatermark(ui->centralwidget->width(),ui->centralwidget->height());
     //WATERMARK_TEXT->setPos(100,100);
     setupFullScreenControls();
 }

 void MainWindow::handleUserManualMaximized(){
     if(fileCount == 0) return;
     //videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     //ui->video_section->fitInView(videoItem, Qt::KeepAspectRatio);
     //ui->video_section->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
//     watermarkHandler->updateWatermark(ui->centralwidget->width(),ui->centralwidget->height());
     //WATERMARK_TEXT->setPos(100,100);
     RECORDING_RED_DOT->updatePosition(20, ui->video_section->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,ui->video_section->width(),ui->video_section->height());
     // this is intentioal dont think it is as redundent
     RECORDING_RED_DOT->updatePosition(20, ui->video_section->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,ui->video_section->width(),ui->video_section->height());
 }

 void MainWindow::handleUserManualUnMaximize(){
     if(fileCount == 0) return;
     //videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     ui->video_section->setGeometry(0, 0, ui->centralwidget->width(), ui->centralwidget->height());
     //videoItem->setSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
     ui->video_section->setGeometry(0, 0, ui->video_section->width(), ui->video_section->width());
     //scene->setSceneRect(0,0,ui->video_section->width(),ui->video_section->width());

     /////////
     RECORDING_RED_DOT->updatePosition(20, ui->video_section->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,ui->video_section->width(),ui->video_section->height());
     RECORDING_RED_DOT->updatePosition(20, ui->video_section->height() - 40);
     RECORDING_FLASH_LAYER->updateSize(0,0,ui->video_section->width(),ui->video_section->height());
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
         break;
     }
 }


 bool MainWindow::event(QEvent *event)  {
     const bool ret_val = QMainWindow::event(event);

     if(!this->videoStarted && event->type() == QEvent::Paint)
    {
        this->videoStarted = true;
        this->manager->GetKeysForChunk(this->token,this->course_id,this->video_id,this->video_item_id);
     }
     return ret_val;
 }

 // testing code


 // Function to extract monitor size from EDID
 bool MainWindow::GetMonitorSizeFromEDID(const HKEY hDevRegKey, short& WidthMm, short& HeightMm)
 {
     DWORD dwType, ActualValueNameLength = NAME_SIZE;
     TCHAR valueName[NAME_SIZE];

     BYTE EDIDdata[1024];
     DWORD edidsize = sizeof(EDIDdata);

     for (LONG i = 0, retValue = ERROR_SUCCESS; retValue != ERROR_NO_MORE_ITEMS; ++i)
     {
         retValue = RegEnumValue(hDevRegKey, i, valueName,
                                 &ActualValueNameLength, NULL, &dwType,
                                 EDIDdata, &edidsize);

         if (retValue != ERROR_SUCCESS || QString::fromWCharArray(valueName) != "EDID")
             continue;

         WidthMm = ((EDIDdata[68] & 0xF0) << 4) + EDIDdata[66];
         HeightMm = ((EDIDdata[68] & 0x0F) << 8) + EDIDdata[67];
         return true; // valid EDID found
     }
     return false; // EDID not found
 }

 // Function to get size for a specific Device ID
 bool MainWindow::GetSizeForDevID(short& WidthMm, short& HeightMm)
 {
     HDEVINFO devInfo = SetupDiGetClassDevsEx(
         &GUID_CLASS_MONITOR, NULL, NULL,
         DIGCF_PRESENT, NULL, NULL, NULL);

     if (devInfo == NULL)
         return false;

     bool bRes = false;
     for (ULONG i = 0; ERROR_NO_MORE_ITEMS != GetLastError(); ++i)
     {
         SP_DEVINFO_DATA devInfoData;
         memset(&devInfoData, 0, sizeof(devInfoData));
         devInfoData.cbSize = sizeof(devInfoData);

         if (SetupDiEnumDeviceInfo(devInfo, i, &devInfoData))
         {
             HKEY hDevRegKey = SetupDiOpenDevRegKey(devInfo, &devInfoData,
                                                    DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
             if (!hDevRegKey || (hDevRegKey == INVALID_HANDLE_VALUE))
                 continue;

             bRes = GetMonitorSizeFromEDID(hDevRegKey, WidthMm, HeightMm);
             RegCloseKey(hDevRegKey);
         }
     }

     SetupDiDestroyDeviceInfoList(devInfo);
     return bRes;
 }

 // Encapsulated function to get monitor siz

 QList<QPair<short, short>> MainWindow::getAllMonitorSizes()
 {
     QList<QPair<short, short>> monitorSizes;

     DISPLAY_DEVICE dd;
     dd.cb = sizeof(dd);
     DWORD dev = 0;

     QString DeviceID;
     while (EnumDisplayDevices(0, dev, &dd, 0))
     {
         DISPLAY_DEVICE ddMon;
         ZeroMemory(&ddMon, sizeof(ddMon));
         ddMon.cb = sizeof(ddMon);
         DWORD devMon = 0;

         while (EnumDisplayDevices(dd.DeviceName, devMon, &ddMon, 0))
         {
             if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE &&
                 !(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
             {
                 DeviceID = QString::fromWCharArray(ddMon.DeviceID);
                 DeviceID = DeviceID.mid(8, DeviceID.indexOf("\\", 9) - 8);

                 short widthMm, heightMm;
                 if (GetSizeForDevID(widthMm, heightMm))
                 {
                     monitorSizes.append(QPair<short, short>(widthMm, heightMm));
                 }
             }
             devMon++;
             ZeroMemory(&ddMon, sizeof(ddMon));
             ddMon.cb = sizeof(ddMon);
         }

         ZeroMemory(&dd, sizeof(dd));
         dd.cb = sizeof(dd);
         dev++;
     }
#ifdef TEST
     monitorSizes.removeFirst();
     return monitorSizes;
#else
     return monitorSizes;
#endif
 }

 // testing code ends

 void MainWindow::onKeyFetchCompleted(QList<VideoData> keyList,MizuConfig * config,int fullVideoDuration){

     //qDebug()<<"monitors info -- " << info;
     ScreenDetector detector = ScreenDetector(this);

     this->playbackTimer = new UserPlaybackTimerTracker(Player,config->log_activity_interval,this);
    // Connect to the play time updated signal
    connect(playbackTimer, &UserPlaybackTimerTracker::sendMetrics, this,&MainWindow::sendTimeToServer);

     this->fullVideoDuration = fullVideoDuration * 1000;
     int monitorCount = detector.getMonitorInfoFromDeviceManager();
     QList<QPair<short, short>> allMonitors =  getAllMonitorSizes();
     if(allMonitors.length() == 1 && monitorCount == 1){
           videoTimeArray  = QList<qint64>();
           for (const VideoData &data : keyList) {
                videoTimeArray.append(data.duration.toInt() * 1000);
           }
           this->videoItemList = keyList;
           durationAverageInSeconds = videoItemList.size() == 1? getSumOfAllVideosTimeTillNow(videoItemList.size() - 1): getSumOfAllVideosTimeTillNow(videoItemList.size() - 1) / videoItemList.size() - 1;
           //durationAverageInSeconds = ;
           on_actionOpen_triggered(config);
           manager->getSessionId(token,course_id,video_item_id,video_id);

            //qDebug()<<videoTimeArray << "video time array -- duration";
     }else {
         QTimer::singleShot(0, this, [this]() {
            showWarningDialog();
         });
     }
 }


 void MainWindow::closeNoInternetDialogAndRetry(bool closeWindow){
     if(closeWindow){
        guiInstance->exit();
     }

     if(noIntentDialog != nullptr){
         manager->GetKeysForChunk(this->token,this->course_id,this->video_id,this->video_item_id);
     }
 }

 void MainWindow::onNoInternet(QString message){
     //qWarning()<<"in no internet";
     noIntentDialog = new NoInternetDialog(this,message);
     noIntentDialog->show();
     connect(noIntentDialog,&NoInternetDialog::onNoInternetDialogClose,this,&MainWindow::closeNoInternetDialogAndRetry);
 }

 void MainWindow::userPlaytimeDataFailed(QString message){
     if(this->retryCounter > 0){
         //qWarning()<<"failed to send user data";
         if(this->playbackTimer != nullptr){
             QTimer::singleShot(this->retryInterval, this, [this]() {
                 QList<qint64> pendingItems = playbackTimer->getPendingItemList();
                 if(pendingItems.length() > 0){
                qint64 currentSeekbarPostion = getCurrentSeekabrPosition();
                qint64 lastPendingItem = pendingItems.first();
                manager->sendUserWatchTime(token,course_id,video_item_id,video_id,lastPendingItem,currentSeekbarPostion);
                this->retryCounter--;
                 }
            });
         }else {

         }
        return;
     }


     Player->pause();
     pausedForNoInternet = true;
     if(IS_FULL_SCREEN){
         fsSeekbar->setDisabled(true);
     }else {
        ui->horizontalSlider_Duration->setDisabled(true);
     }
     handlePlayPauseButtonState(Player->playPauseState());
     noIntentDialogForTimer = new NoInternetDialog(this,message);
     noIntentDialogForTimer->show();
     connect(noIntentDialogForTimer,&NoInternetDialog::onNoInternetDialogClose,this,&MainWindow::closeWatchTimeNoInternetDialogAndRetry);
 }

 void MainWindow::closeWatchTimeNoInternetDialogAndRetry(bool closeWindow){
     qDebug()<<"close window data --- " << closeWindow;
     if(closeWindow){
         guiInstance->exit();
         return;
     }

     if(noIntentDialogForTimer != nullptr){
         QList<qint64> pendingItems = playbackTimer->getPendingItemList();
         if(pendingItems.length() > 0){
            qint64 currentSeekbarPostion = getCurrentSeekabrPosition();
            qint64 lastPendingItem = pendingItems.first();
            manager->sendUserWatchTime(token,course_id,video_item_id,video_id,lastPendingItem,currentSeekbarPostion);
         }
     }
 }

 qint64 MainWindow::getCurrentSeekabrPosition(){
    qint64 currentSeekbarPostion = 0;
     try{
        if(IS_FULL_SCREEN){
            currentSeekbarPostion = fsSeekbar->value() * 1000;
        }else {
            currentSeekbarPostion = ui->horizontalSlider_Duration->value() * 1000;
        }
    }catch (const std::exception &e){
         qDebug()<<e.what();
     }
    return currentSeekbarPostion;
 }

 void MainWindow::userPlaytimeDataSuccess(){
     this->retryCounter = 3;
     playbackTimer->removeLastPendingItem();
     if(IS_FULL_SCREEN){
         fsSeekbar->setDisabled(false);
     }else {
        ui->horizontalSlider_Duration->setDisabled(false);
     }
     if(Player->playPauseState() == PlayPauseState::Pause && pausedForNoInternet){
        Player->play();
        pausedForNoInternet = false;
     }

     auto monitorSizes = getAllMonitorSizes();
     ScreenDetector detector  = ScreenDetector();
     if(monitorSizes.length() > 1 || detector.getMonitorInfoFromDeviceManager() > 1){
         QTimer::singleShot(0, this, [this]() {
            showWarningDialog();
         });
     }
 }


 void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
 {
     float requiredSpeed = Player->playbackRate();
     if(arg1 == "1x"){
         requiredSpeed = 1.0f;

     }else if(arg1 == "1.2x"){
         requiredSpeed = 1.2f;

     }else if(arg1 == "1.5x"){
         requiredSpeed = 1.5f;

     }else if(arg1 == "1.75x"){
         requiredSpeed = 1.75f;

     }else if(arg1 == "2.0x"){
         requiredSpeed = 2.0f;

     }else if(arg1 == "2.5x"){
         requiredSpeed = 2.5f;
     }
     Player->changeSpeed(requiredSpeed);
     playbackRateHandler->chnagePlaybackRate(requiredSpeed);
     if(IS_FULL_SCREEN){
         this->fsSpeedBox->clearFocus();
     }else {
         ui->comboBox->clearFocus();
     }
 }

