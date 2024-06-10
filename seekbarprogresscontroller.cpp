#include "seekbarprogresscontroller.h"

SeekbarProgressController::SeekbarProgressController(QObject *parent) : QObject {parent} {
    this->globalPlaybackTimer = new  QTimer();
    this->globalPlaybackTimer->setInterval(1000);
    this->timerConnection = connect(this->globalPlaybackTimer,&QTimer::timeout,this,&SeekbarProgressController::onTick);
    this->wasPaused = false;
}
SeekbarProgressController::~SeekbarProgressController(){
    this->globalPlaybackTimer->stop();
    this->currentVideoTimeInSeconds = 0;
    emit this->timeMovedTo(0);
}

void SeekbarProgressController::onTick(){
    if(this->markMooving) return;
    this->currentVideoTimeInSeconds++;
    emit timeMovedTo(this->currentVideoTimeInSeconds);
    // emit
    if(wasPaused){
        this->globalPlaybackTimer->start(1000);
    }
}

void SeekbarProgressController::startTimer(){
    this->globalPlaybackTimer->start();
}

void SeekbarProgressController::stop(){
    this->globalPlaybackTimer->stop();
    this->currentVideoTimeInSeconds = 0;
    emit timeMovedTo(this->currentVideoTimeInSeconds);
}

void SeekbarProgressController::pause(){
    this->remainingTimeOnPause = this->globalPlaybackTimer->remainingTime();
    this->globalPlaybackTimer->stop();
    this->wasPaused = true;
}

void SeekbarProgressController::resume(){
    this->globalPlaybackTimer->start(this->remainingTimeOnPause);
}
void SeekbarProgressController::moveTo(qint64 secondsToJump){
    this->globalPlaybackTimer->stop();
    this->currentVideoTimeInSeconds = secondsToJump;
    emit timeMovedTo(this->currentVideoTimeInSeconds);
    this->startTimer();
}

void SeekbarProgressController::markGettingDragged(){
    this->markMooving = true;
}

void SeekbarProgressController::markDraggingEnded(){
    this->markMooving = false;
}
