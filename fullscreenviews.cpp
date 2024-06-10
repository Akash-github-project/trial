#include "fullscreenviews.h"



// FullScreenViews::FullScreenViews(QObject *parent,Ui::MainWindow *mainWindow)
//     : QObject{parent}
// {
//     //don't know wether we need this full screen
//     this->isFullScreen = true;
//     loadFullScreenControls(mainWindow,window);
// }


// void FullScreenViews::loadFullScreenControls(Ui::MainWindow *ui,QMainWindow *window){
//      QUiLoader loader;
//      QFile file(":/controls_overlay.ui"); // Ensure the path to your .ui file is correct
//      file.open(QFile::ReadOnly);
//      controls = loader.load(&file);
//      file.close();
//      // Find the controls in the loaded UI

//      // Add the UI as a proxy widget to the scene
//      QGraphicsProxyWidget *proxy = scene->addWidget(controls);
//      //proxy->setPos(0, ui->centralwidget->height() - 300); // Adjust position as necessary
//      proxy->setGeometry(QRect(0,ui->centralwidget->height() - 200,ui->centralwidget->width(),200));
//      //proxy->setOpacity(0);

//      fsPlayPauseButton = controls->findChild<QPushButton*>("fs_PushButton_Play_Pause",Qt::FindChildOption::FindChildrenRecursively);
//      fsStopButton = controls->findChild<QPushButton*>("fs_PushButton_Stop",Qt::FindChildOption::FindChildrenRecursively);
//      fsSeekbar = controls->findChild<QSlider*>("fs_HorizontalSlider_Duration",Qt::FindChildOption::FindChildrenRecursively);
//      fsNormalButton = controls->findChild<QPushButton*>("normal_screen_button",Qt::FindChildOption::FindChildrenRecursively);
//      fsTotalTime = controls->findChild<QLabel*>("fs_label_total_time",Qt::FindChildOption::FindChildrenRecursively);
//      fsCurrentTime = controls->findChild<QLabel*>("fs_label_current_time",Qt::FindChildOption::FindChildrenRecursively);
//      fsTenSecForward = controls->findChild<QPushButton*>("fs_PushButton_Seek_Forward",Qt::FindChildOption::FindChildrenRecursively);
//      fsTenSecBackward = controls->findChild<QPushButton*>("fs_PushButton_Seek_Backward",Qt::FindChildOption::FindChildrenRecursively);
//      fsSpeed1x = controls->findChild<QPushButton*>("pushButton_1x",Qt::FindChildOption::FindChildrenRecursively);
//      fsSpeed1p2x = controls->findChild<QPushButton*>("pushButton_1p2x",Qt::FindChildOption::FindChildrenRecursively);
//      fsSpeed1p5x = controls->findChild<QPushButton*>("pushButton_1p5x",Qt::FindChildOption::FindChildrenRecursively);
//      fsSpeed2x = controls->findChild<QPushButton*>("pushButton_2x",Qt::FindChildOption::FindChildrenRecursively);
//      fsSeekbarVolume = controls->findChild<QSlider*>("fs_HorizontalSlider_Volume",Qt::FindChildOption::FindChildrenRecursively);
//      // Connect the buttons to the media player
//      /// \brief connect
//      QGraphicsRectItem* hoverArea = new FullScreenControlHoverHandler(proxy, QRectF(0,ui->centralwidget->height() - 200,ui->centralwidget->width(),200));
//      hoverArea->setPen(Qt::NoPen);
//      hoverArea->setAcceptHoverEvents(true);
//      window->scene->addItem(hoverArea);
//      ///
//      connect(fsPlayPauseButton, &QPushButton::clicked, window, &MainWindow::on_pushButton_Play_Pause_clicked);
//      connect(fsStopButton, &QPushButton::clicked, window, &MainWindow::on_pushButton_Stop_clicked);
//      connect(fsNormalButton,&QPushButton::clicked,window, &MainWindow::on_normal_button_pressed);
//      ///
//     fsSeekbarController = new VideoProgressBarController(fsSeekbar,window->fileCount);
//     fsSeekbarController->setSliderMaxLimit(window->timeLimit);
//     fsSeekbarConnection = connect(fsSeekbarController,&VideoProgressBarController::onSeekbarStopedSliding,window,&MainWindow::onSliderStop);
//     fsSeekbarForwardBackwardConnection = connect(fsSeekbarController,&VideoProgressBarController::onSeekbarSecondsTimerEndSliding,window,&MainWindow::seekBackward);
//     connect(fsSeekbar,&QSlider::sliderReleased,window,&MainWindow::on_horizontalSlider_Duration_sliderMoved);
//     connect(fsSeekbar,&QSlider::sliderPressed,window,&MainWindow::slderClicked);
//     connect(fsTenSecForward, &QPushButton::clicked,window,&MainWindow::on_pushButton_Seek_Forward_clicked);
//     connect(fsTenSecBackward, &QPushButton::clicked,window,&MainWindow::on_pushButton_Seek_Backward_clicked);

//     window->handlePlayPauseButtonState(window->Player->playbackState());
//     connect(fsSpeed1x,&QPushButton::clicked,window,&MainWindow::on_pushButton_1x_clicked);
//     connect(fsSpeed1p2x,&QPushButton::clicked,window,&MainWindow::on_pushButton_1p2x_clicked);
//     connect(fsSpeed1p5x,&QPushButton::clicked,window,&MainWindow::on_pushButton_1p5x_clicked);
//     connect(fsSpeed2x,&QPushButton::clicked,window,&MainWindow::on_pushButton_2x_clicked);
//     window->onPlaybackRateChanged(window->Player->playbackRate());
//     fsSeekbarVolume->setMaximum(100);
//     fsSeekbarVolume->setMinimum(0);
//     fsSeekbarVolume->setValue(ui->horizontalSlider_Volume->value());
//     connect(fsSeekbarVolume, &QSlider::valueChanged,window,&MainWindow::handleVolumeChange);
// }

// void FullScreenViews::goBackToNormalScreen(Ui::MainWindow *ui,MainWindow *window){
//      window->scene->removeItem(controls->graphicsProxyWidget());
//      window->setWindowState(Qt::WindowNoState);
//      ui->controlsSection->show();
//      window->videoItem->setSize(QSize(window->oldWidth, window->oldHeight));
//      window->view->fitInView(window->videoItem, Qt::KeepAspectRatio);
//      window->view->setGeometry(0, 0, window->oldWidth, window->oldHeight);
//      window->videoItem->setSize(QSize(window->oldWidth, window->oldHeight));
//      window->view->fitInView(window->videoItem, Qt::KeepAspectRatio);
//      window->view->setGeometry(0, 0, window->oldHeight, window->oldHeight);
//      window->playbackRateHandler->chnagePlaybackRate(window->Player->playbackRate());
//      ui->horizontalSlider_Volume->setValue(fsSeekbarVolume->value());
//      window->handlePlayPauseButtonState(window->Player->playbackState());

//      QObject::disconnect(fsSeekbarConnection);
//      QObject::disconnect(fsPlayPauseButton);
//      QObject::disconnect(fsStopButton);
//      QObject::disconnect(fsNormalButton);
//      QObject::disconnect(fsSeekbar);
//      QObject::disconnect(fsSpeed1x);
//      QObject::disconnect(fsSpeed1p2x);
//      QObject::disconnect(fsSpeed1p5x);
//      QObject::disconnect(fsSpeed2x);
//      QObject::disconnect(fsSeekbarForwardBackwardConnection);
// }


