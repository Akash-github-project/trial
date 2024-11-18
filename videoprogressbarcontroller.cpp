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
//    if(blocked == false){
        horizontalSlider_Duration->setValue(moveTo);
 //   }
}

void VideoProgressBarController::setCurrentIndex(int index){
    currentIndex = index;
}

void VideoProgressBarController::setSliderMaxLimit(qint64 maxDuration){
    videoCompleteDuration = maxDuration * videosCount;
    horizontalSlider_Duration->setMaximum(maxDuration);
    horizontalSlider_Duration->setRange(0, maxDuration);
    qDebug()<<"current duration -------- ......." << maxDuration;
}

void VideoProgressBarController::updateMaxLimit(qint64 maxDuration,int videosCount){
    videoCompleteDuration = maxDuration * videosCount;
    horizontalSlider_Duration->setMaximum(videoCompleteDuration);
    horizontalSlider_Duration->setRange(0, videoCompleteDuration);
    horizontalSlider_Duration->update();
}

void VideoProgressBarController::setupTimer(QObject *parent){
    if(horizontalSlider_Duration->isSliderDown()){
        return;
    }
    emit onSeekbarStopedSliding();
}

void  VideoProgressBarController::jumpInstantly(QObject *parent){
    emit onSeekbarStopedSliding();
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
}

void VideoProgressBarController::markSeekPending(bool pendingStatus){
    this->seekPending = pendingStatus;
}

void VideoProgressBarController::onSeekbarSecondsTimerEnd(){
    emit onSeekbarSecondsTimerEndSliding(oldTime);
}

void VideoProgressBarController::onSeekbarDebounceTimerEnd(){
    qDebug()<<"emitted";
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
