#include "playbackratehandler.h"
PlaybackRateHandler::PlaybackRateHandler(QObject *parent): QObject{parent}{

}

PlaybackRateHandler::PlaybackRateHandler(QObject *parent,VLCPlayer *player)
    : QObject{parent}
{
    mediaPlayerInstance = player;
}

void PlaybackRateHandler::chnagePlaybackRate(float playbackRate){
    mediaPlayerInstance->changeSpeed(playbackRate);
    //mediaPlayerInstance->setPlaybackRate(playbackRate);
    emit playbackRateChanged(playbackRate);
}
