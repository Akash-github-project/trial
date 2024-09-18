// UserPlaybackTimerTracker.cpp
#include "UserPlaybackTimerTracker.h"

UserPlaybackTimerTracker::UserPlaybackTimerTracker(QMediaPlayer *player, QObject *parent)
    : QObject(parent),
    m_updateTimer(new QTimer(this)),
    m_serverUpdateTimer(new QTimer(this)),
    m_accumulatedTime(0),
    m_isRunning(false),
    m_mediaPlayer(player) {

    // Connect the update timer to update play time every second
    connect(m_updateTimer, &QTimer::timeout, this, &UserPlaybackTimerTracker::updatePlayTime);
    m_updateTimer->setInterval(1000); // Update every second

    // Connect the media player state change signal to handle play, pause, and stop
    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &UserPlaybackTimerTracker::onMediaPlayerStateChanged);

    // Set up the server update timer to emit metrics signal every minute
    connect(m_serverUpdateTimer, &QTimer::timeout, this, &UserPlaybackTimerTracker::emitMetricsSignal);
    m_serverUpdateTimer->start(60000); // Emit metrics signal every 60 seconds
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
        return (m_accumulatedTime + m_elapsedTimer.elapsed()) / 1000; // Convert milliseconds to seconds
    } else {
        // Return accumulated time when paused
        return m_accumulatedTime / 1000; // Convert milliseconds to seconds
    }
}

void UserPlaybackTimerTracker::updatePlayTime() {
    emit playTimeUpdated(getTotalPlayTime());
}

void UserPlaybackTimerTracker::onMediaPlayerStateChanged(QMediaPlayer::PlaybackState state) {
    switch (state) {
    case QMediaPlayer::PlayingState:
        if (!m_isRunning) {
            m_elapsedTimer.start();
            m_updateTimer->start();
            m_isRunning = true;
        }
        break;
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
        if (m_isRunning) {
            m_accumulatedTime += m_elapsedTimer.elapsed();
            m_updateTimer->stop();
            m_isRunning = false;
        }
        break;
    default:
        break;
    }
}

void UserPlaybackTimerTracker::emitMetricsSignal() {
    qint64 playTimeInSeconds =  getTotalPlayTime() == 60? 60:getTotalPlayTime() % 60; // Ensure value is within 0-60 seconds
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
}

void UserPlaybackTimerTracker::removeLastPendingItem() {
    this->m_pendingTimerSent.pop_back();
}

QList<qint64> UserPlaybackTimerTracker::getPendingItemList() {
    return this->m_pendingTimerSent;
}
