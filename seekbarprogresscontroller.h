#ifndef SEEKBARPROGRESSCONTROLLER_H
#define SEEKBARPROGRESSCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QDebug>

class SeekbarProgressController : public QObject
{
    Q_OBJECT


public:
    explicit SeekbarProgressController(QObject *parent = nullptr);
    ~SeekbarProgressController();
    void startTimer();
    void resume();
    void pause();
    void stop();
    void moveTo(qint64 secondsToJump);
    void markGettingDragged();
    void markDraggingEnded();
private:
    QTimer* globalPlaybackTimer;
    qint64 currentVideoTimeInSeconds = -1;
    QMetaObject::Connection timerConnection;
    qint64 remainingTimeOnPause;
    bool wasPaused;
    bool markMooving = false;
signals:
    void timeMovedTo(qint64 currentTimeOfSeekbar);
private slots:
    void onTick();
};

#endif // SEEKBARPROGRESSCONTROLLER_H
