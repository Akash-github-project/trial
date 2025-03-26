// UserPlaybackTimerTracker.cpp
#include "UserPlaybackTimerTracker.h"
#include <QDebug>

UserPlaybackTimerTracker::UserPlaybackTimerTracker(VLCPlayer *player,int callbackDuration ,QObject *parent)
    : QObject(parent),
    m_updateTimer(new QTimer(this)),
    m_serverUpdateTimer(new QTimer(this)),
    m_accumulatedTime(0),
    m_isRunning(false),
    m_mediaPlayer(player) {

    try {
        this->durationToSend = callbackDuration;
        // Connect the update timer to update play time every second
        connect(m_updateTimer, &QTimer::timeout, this, &UserPlaybackTimerTracker::updatePlayTime);
        m_updateTimer->setInterval(1000); // Update every second
        m_updateTimer->start();
        // Connect the media player state change signal to handle play, pause, and stop
        connect(m_mediaPlayer, &VLCPlayer::mediaStatusChanged, this, &UserPlaybackTimerTracker::onMediaPlayerStateChanged);

        // Set up the server update timer to emit metrics signal every minute
        connect(m_serverUpdateTimer, &QTimer::timeout, this, &UserPlaybackTimerTracker::emitMetricsSignal);
        m_serverUpdateTimer->start(this->durationToSend * 1000); // Emit metrics signal every 60 seconds
    }catch(const std::exception& e){
        qWarning()<<"what :" << e.what();
    }
}

UserPlaybackTimerTracker::~UserPlaybackTimerTracker() {
    delete m_updateTimer;
    delete m_serverUpdateTimer;
}

void UserPlaybackTimerTracker::reset() {
    m_accumulatedTime = 0;
    m_isRunning = false;
    m_updateTimer->stop();
}

qint64 UserPlaybackTimerTracker::getTotalPlayTime() const {
    if (m_isRunning) {
        // Add elapsed time to accumulated time when running
        //qDebug()<<"play time "<<m_accumulatedTime + m_elapsedTimer.elapsed();
        return (m_accumulatedTime + m_elapsedTimer.elapsed()) / 1000; // Convert milliseconds to seconds
    } else {
        // Return accumulated time when paused
        //qDebug()<<"play time 2"<<m_accumulatedTime;
        return m_accumulatedTime / 1000; // Convert milliseconds to seconds
    }
}

void UserPlaybackTimerTracker::updatePlayTime() {
    //qDebug()<<"starting the timer" ;
    if (m_mediaPlayer->playbackState() == PlaybackState::Playing) {
        m_accumulatedTime += 1000;
        //qWarning()<<"adding 1000" << m_accumulatedTime <<"final value";
    }
    emit playTimeUpdated(getTotalPlayTime());
}

void UserPlaybackTimerTracker::onMediaPlayerStateChanged(PlaybackState state) {
    switch (state) {
    case PlaybackState::Playing:
        if (!m_isRunning) {
            m_elapsedTimer.start();
            m_updateTimer->start();
            m_isRunning = true;
        }
        break;
    case PlaybackState::Paused:
    case PlaybackState::Ended:
        if (m_isRunning) {
            //m_accumulatedTime += m_elapsedTimer.elapsed();
            m_updateTimer->stop();
            m_isRunning = false;
        }
        break;
    default:
        break;
    }
}

void UserPlaybackTimerTracker::emitMetricsSignal() {
    try {
        qDebug()<<"getTotalPlayTime" << getTotalPlayTime();
        qDebug()<<"durationToSend" << this->durationToSend;
        qint64 playTimeInSeconds =  getTotalPlayTime() == this->durationToSend? this->durationToSend:getTotalPlayTime() % this->durationToSend; // Ensure value is within 0-60 seconds
        if(playTimeInSeconds == 0 && this->m_pendingTimerSent.length() == 0){
            return;
        }
        if(playTimeInSeconds == 0 && this->m_pendingTimerSent.length() != 0){
            emit sendMetrics(this->m_pendingTimerSent.first());
        }else {
            emit sendMetrics(playTimeInSeconds);
            this->m_pendingTimerSent.append(playTimeInSeconds);
        }
        // Reset the accumulated time after sending the metrics
        m_accumulatedTime = 0;
        m_elapsedTimer.restart(); // Restart the timer for the next period
    }catch(const std::exception& e){
        qWarning()<< "what: "<<e.what();
    }
}

void UserPlaybackTimerTracker::removeLastPendingItem() {
    this->m_pendingTimerSent.pop_back();
}

QList<qint64> UserPlaybackTimerTracker::getPendingItemList() {
    if(this->m_pendingTimerSent.isEmpty()){
        this->m_pendingTimerSent.append(0);
    }
    return this->m_pendingTimerSent;
}
