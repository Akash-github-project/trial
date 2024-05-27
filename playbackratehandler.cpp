#include "playbackratehandler.h"
PlaybackRateHandler::PlaybackRateHandler(QObject *parent): QObject{parent}{

}

PlaybackRateHandler::PlaybackRateHandler(QObject *parent,QMediaPlayer *player)
    : QObject{parent}
{
    mediaPlayerInstance = player;
}

void PlaybackRateHandler::chnagePlaybackRate(float playbackRate){
    mediaPlayerInstance->setPlaybackRate(playbackRate);
    emit playbackRateChanged(playbackRate);
}
