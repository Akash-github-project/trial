#include "videoprogressbarcontroller.h"


VideoProgressBarController::VideoProgressBarController() {  }


VideoProgressBarController::VideoProgressBarController(QSlider *slider,int videoCount){
    horizontalSlider_Duration = slider;
    videosCount = videoCount;
}

VideoProgressBarController::~VideoProgressBarController(){
    free(horizontalSlider_Duration);
}

//resetting section
void VideoProgressBarController:: resetCurrentIndex(){
    currentIndex = 0;
}

void VideoProgressBarController::resetSlider(){
    sliderTime = -1;
}
void VideoProgressBarController::resetSliderTime(){
    horizontalSlider_Duration->setValue(0);
}

//
void VideoProgressBarController::moveSlider(int moveTo){
    if(blocked == false){
        horizontalSlider_Duration->setValue(moveTo);
    }
}

void VideoProgressBarController::setCurrentIndex(int index){
    currentIndex = index;
}

void VideoProgressBarController::setSliderMaxLimit(qint64 maxDuration){
    videoCompleteDuration = maxDuration * videosCount;
    horizontalSlider_Duration->setMaximum(videoCompleteDuration);
    horizontalSlider_Duration->setRange(0, videoCompleteDuration);
}

void VideoProgressBarController::updateMaxLimit(qint64 maxDuration,int videosCount){
    videoCompleteDuration = maxDuration * videosCount;
    horizontalSlider_Duration->setMaximum(videoCompleteDuration);
    horizontalSlider_Duration->setRange(0, videoCompleteDuration);
    horizontalSlider_Duration->update();
}

void VideoProgressBarController::setupTimer(QObject *parent){ if(jumpedJustNow){
        jumpedJustNow = false;
        return;
    }
    if(debounceTimer == nullptr){
        debounceTimer =  new QTimer(parent);
        debounceTimer->setSingleShot(true);
        debounceTimer->setInterval(30);
        connect(debounceTimer,&QTimer::timeout,this,&VideoProgressBarController::onSeekbarDebounceTimerEnd);
        debounceTimer->start();
    }else {
        debounceTimer->stop();
        debounceTimer->setInterval(30);
        debounceTimer->start(30);
    }
    qDebug()<<"setuped timer";
}

void  VideoProgressBarController::jumpInstantly(QObject *parent){
    if(debounceTimer == nullptr){
        debounceTimer =  new QTimer(parent);
        debounceTimer->setSingleShot(true);
        debounceTimer->setInterval(1);
        connect(debounceTimer,&QTimer::timeout,this,&VideoProgressBarController::onSeekbarDebounceTimerEnd);
        jumpedJustNow = true;
        debounceTimer->start();
    }else {
        debounceTimer->stop();
        debounceTimer->start(1);
    }
    qDebug()<<"setuped jump instantly";
}

void VideoProgressBarController::setupSeekTimer(QObject *parent,long oldTime){
    this->oldTime = oldTime;
    if(seekDebounceTimer == nullptr){
        seekDebounceTimer =  new QTimer(parent);
        seekDebounceTimer->setSingleShot(true);
        seekDebounceTimer->setInterval(400);
        connect(seekDebounceTimer,&QTimer::timeout,this,&VideoProgressBarController::onSeekbarSecondsTimerEnd);
        seekDebounceTimer->start();
    }else {
        seekDebounceTimer->stop();
        seekDebounceTimer->setSingleShot(true);
        seekDebounceTimer->start(400);
    }
    qDebug()<<"setuped timer";
}

void VideoProgressBarController::onSeekbarSecondsTimerEnd(){
    emit onSeekbarSecondsTimerEndSliding(oldTime);
}

void VideoProgressBarController::onSeekbarDebounceTimerEnd(){
    qDebug()<<"emitted";
    emit onSeekbarStopedSliding();
}

int VideoProgressBarController::getValue(){
    return horizontalSlider_Duration->value();
}
int VideoProgressBarController::getSliderPosition(){
    return horizontalSlider_Duration->sliderPosition();
}

void VideoProgressBarController::scheduleSeek(long long seek){
    extraSeek = seek;
}
