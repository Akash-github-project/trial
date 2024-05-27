#ifndef PLAYBACKRATEHANDLER_H
#define PLAYBACKRATEHANDLER_H

#include <QMediaPlayer>
#include <QObject>

class PlaybackRateHandler : public QObject
{
    Q_OBJECT
private:
    float playbackRate = 1.0f;
    QMediaPlayer *mediaPlayerInstance;
public:
    explicit PlaybackRateHandler(QObject *parent = nullptr);
    PlaybackRateHandler(QObject *parent = nullptr,QMediaPlayer *player = nullptr);
    void chnagePlaybackRate(float playbackRate);

signals:
    void playbackRateChanged(float playbackRate);

};

#endif // PLAYBACKRATEHANDLER_H
