// UserPlaybackTimerTracker.h
#ifndef USERPLAYBACKTIMERTRACKER_H
#define USERPLAYBACKTIMERTRACKER_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QMediaPlayer>
#include <QList>
#include <vlcplayer.h>


class UserPlaybackTimerTracker : public QObject {
    Q_OBJECT

public:
    explicit UserPlaybackTimerTracker(VLCPlayer *player,int callbackDuration,QObject *parent = nullptr);
    ~UserPlaybackTimerTracker();
    // Reset the timer
    void reset();
    // Get the total play time in seconds
    qint64 getTotalPlayTime() const;
    QList<qint64> getPendingItemList();
    void removeLastPendingItem();
signals:
    void playTimeUpdated(qint64 seconds);
    void sendMetrics(qint64 playTimeInSeconds); // Signal to notify when to send metrics

private slots:
    void updatePlayTime();
    void onMediaPlayerStateChanged(PlaybackState state);

public slots:
    void emitMetricsSignal(); // Slot to trigger the metrics signal

private:
    QTimer* m_updateTimer;           // Timer for periodic updates
    QTimer* m_serverUpdateTimer;     // Timer to emit metrics signal every minute
    QElapsedTimer m_elapsedTimer;    // Tracks actual elapsed time
    qint64 m_accumulatedTime;        // Accumulated play time in milliseconds
    bool m_isRunning;                // Tracks whether the timer is running
    VLCPlayer* m_mediaPlayer;     // Pointer to the QMediaPlayer
    QList<qint64> m_pendingTimerSent;
    int durationToSend = 60;
};

#endif // USERPLAYBACKTIMERTRACKER_H
