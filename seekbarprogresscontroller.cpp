#include "seekbarprogresscontroller.h"

SeekbarProgressController::SeekbarProgressController(QObject *parent) : QObject {parent} {
    this->globalPlaybackTimer = new  QTimer();
    this->globalPlaybackTimer->setInterval(1000);
    this->timerConnection = connect(this->globalPlaybackTimer,&QTimer::timeout,this,&SeekbarProgressController::onTick);
    this->wasPaused = false;
    //qDebug()<<"wasPaused - " <<wasPaused;
}
SeekbarProgressController::~SeekbarProgressController(){
    this->globalPlaybackTimer->stop();
    //qDebug()<<"globalPlaybackTimer - " <<globalPlaybackTimer->isActive();
    this->currentVideoTimeInSeconds = 0;
    //qDebug()<<"currentVideoTimeInSeconds - " <<currentVideoTimeInSeconds;
    emit this->timeMovedTo(0);
}

void SeekbarProgressController::onTick(){
    //qDebug()<<"markMooving - " <<markMooving;
    if(this->markMooving) return;
    //qDebug()<<"currentVideoTimeInSeconds - " <<currentVideoTimeInSeconds;
    this->currentVideoTimeInSeconds++;
    emit timeMovedTo(this->currentVideoTimeInSeconds);
    //qDebug()<<"currentVideoTimeInSeconds - 2" <<currentVideoTimeInSeconds;
    // emit
    //qDebug()<<"wasPaused - 2" <<wasPaused;
    if(wasPaused){
        this->globalPlaybackTimer->start(1000);
        //qDebug()<<"globalPlaybackTimer - 2" <<globalPlaybackTimer->isActive();
    }
}


void SeekbarProgressController::startTimer(){
    //qDebug()<<"globalPlaybackTimer - 3" <<"started timer";
    this->globalPlaybackTimer->start();
}

void SeekbarProgressController::stop(){
    //qDebug()<<"globalPlaybackTimer - 3" <<"stop timer";
    this->globalPlaybackTimer->stop();
    //qDebug()<<"currentVideoTimeInSeconds - 4" <<currentVideoTimeInSeconds;
    this->currentVideoTimeInSeconds = 0;
    //qDebug()<<"currentVideoTimeInSeconds - 5" <<currentVideoTimeInSeconds;
    emit timeMovedTo(this->currentVideoTimeInSeconds);
}

void SeekbarProgressController::pause(){
    //qDebug()<<"globalPlaybackTimer - 5" <<globalPlaybackTimer->isActive();
    this->remainingTimeOnPause = this->globalPlaybackTimer->remainingTime();
    //qDebug()<<"globalPlaybackTimer - 6" <<globalPlaybackTimer->isActive();
    this->globalPlaybackTimer->stop();
    //qDebug()<<"globalPlaybackTimer - 7" <<globalPlaybackTimer->isActive();
    this->wasPaused = true;
    //qDebug()<<"wasPaused - 4" <<wasPaused;
}

void SeekbarProgressController::resume(){
    this->globalPlaybackTimer->start(this->remainingTimeOnPause);
    //qDebug()<<"globalPlaybackTimer - 8" <<globalPlaybackTimer->isActive();
}
void SeekbarProgressController::moveTo(qint64 secondsToJump){
    this->globalPlaybackTimer->stop();
    //qDebug()<<"globalPlaybackTimer - 9" <<globalPlaybackTimer->isActive();
    this->currentVideoTimeInSeconds = secondsToJump;
    //qDebug()<<"currentVideoTimeInSeconds - 4" <<currentVideoTimeInSeconds;
    emit timeMovedTo(this->currentVideoTimeInSeconds);
    this->startTimer();
    //qDebug()<<"Timer start";
}

void SeekbarProgressController::markGettingDragged(){
    this->markMooving = true;
    //qDebug()<<"markMooving"<<markMooving;
}

void SeekbarProgressController::markDraggingEnded(){
    this->markMooving = false;
    //qDebug()<<"markMooving 2"<<markMooving;
}
