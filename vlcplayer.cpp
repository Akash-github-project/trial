#include "vlcplayer.h"

extern "C" {
// Lock callback: Provide a buffer for libVLC to write the video frame
void* lock_callback(void* opaque, void** planes) {
    VlcVideoData* data = static_cast<VlcVideoData*>(opaque);
    if (!data) return nullptr;

    // Lock the mutex to protect access to the frame
    data->mutex.lock();

    // Provide the buffer where libVLC will write the frame
    *planes = data->frame.bits();

    return nullptr; // Picture user data (not used here)
}

// Unlock callback: Called after libVLC has written the frame
void unlock_callback(void* opaque, void* picture, void* const* planes) {
    Q_UNUSED(picture);
    Q_UNUSED(planes);

    VlcVideoData* data = static_cast<VlcVideoData*>(opaque);
    if (!data) return;

    // Unlock the mutex as we're done writing to the frame
    data->mutex.unlock();
}

// Display callback: Handle the rendered frame (e.g., update the VideoWidget)
void display_callback(void* opaque, void* picture) {
    Q_UNUSED(picture);

    VlcVideoData* data = static_cast<VlcVideoData*>(opaque);
    if (!data) return;

    // Emit a signal to update the VideoWidget with the new frame
    // Since libVLC may call this from a different thread, use Qt's signal-slot mechanism
    QMetaObject::invokeMethod(data->videoWidget, "renderFrame", Qt::QueuedConnection, Q_ARG(QImage, data->frame));
}
// video callback ends

// Open callback: Initialize the buffer
int vlc_open_callback(void* data, void** p_datap, uint64_t* sizep) {
    VlcBufferContext* context = static_cast<VlcBufferContext*>(data);
    if (!context || !context->buffer)
        return -1;

    if (!context->buffer->open(QIODevice::ReadOnly))
        return -1;

    *sizep = context->buffer->size();
    *p_datap = data; // Not used in this example
    return 0; // Success
}

// Read callback: Read data from the buffer
ssize_t vlc_read_callback(void* data, unsigned char* buf, size_t len) {
    VlcBufferContext* context = static_cast<VlcBufferContext*>(data);
    if (!context || !context->buffer)
        return 0;

    qint64 bytesRead = context->buffer->read(reinterpret_cast<char*>(buf), len);
    return static_cast<size_t>(bytesRead);
}

// Seek callback: Seek to a position in the buffer
int vlc_seek_callback(void* data, uint64_t offset) {
    VlcBufferContext* context = static_cast<VlcBufferContext*>(data);
    if (!context || !context->buffer)
        return -1;

    if (!context->buffer->seek(offset))
        return -1;

    return 0; // Success
}

// Close callback: Clean up the buffer and context
void vlc_close_callback(void* data) {
    VlcBufferContext* context = static_cast<VlcBufferContext*>(data);
    if (context) {
        // BufferContext destructor handles buffer cleanup
        delete context;
    }
}

void media_event_callback(const libvlc_event_t* event, void* data) {
    VLCPlayer* player = static_cast<VLCPlayer*>(data);
    player->handleMediaEvents(event);
}

}


VLCPlayer::VLCPlayer(QObject *parent)
    : QObject{parent}
{
    // vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    // if (!vlcInstance) { }

    // // Create a new libVLC media player
    // mediaPlayer = libvlc_media_player_new(vlcInstance);
    // if (!mediaPlayer) {
    //     libvlc_release(vlcInstance);
    // }
    // setupPositionChangedListener();

    // media_event_manager = libvlc_media_player_event_manager(mediaPlayer);
    // libvlc_event_attach(media_event_manager, libvlc_MediaPlayerEndReached, media_event_callback, this);

}

VLCPlayer::VLCPlayer(QObject *parent,VideoWidget* videoWidget)
    : QObject{parent}
{
    vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    if (!vlcInstance) { }
    videoData = new VlcVideoData();
    videoData->videoWidget = videoWidget;

    int initialWidth = 1280;
    int initialHeight = 720;
    videoData->frame = QImage(initialWidth,initialHeight,QImage::Format_RGB32);
    videoData->frame.fill(Qt::black); // Initialize with black frame

    // Create a new libVLC media player
    mediaPlayer = libvlc_media_player_new(vlcInstance);
    if (!mediaPlayer) {
        libvlc_release(vlcInstance);
    }
    setupPositionChangedListener();

    media_event_manager = libvlc_media_player_event_manager(mediaPlayer);
    libvlc_event_attach(media_event_manager, libvlc_MediaPlayerEndReached, media_event_callback, this);

    // VLCPlayer.cpp (Within VLCPlayer constructor)
    libvlc_video_set_format(mediaPlayer, "RV32", videoData->frame.width(), videoData->frame.height(), videoData->frame.bytesPerLine());
    libvlc_video_set_callbacks(mediaPlayer, lock_callback, unlock_callback, display_callback, videoData);

}



VLCPlayer::~VLCPlayer(){
    if (mediaPlayer) {
        libvlc_media_player_release(mediaPlayer);
    }
    if (!mediaPlayer) {
        //QMessageBox::critical(this, "Error", "Failed to create libVLC media player");
        libvlc_release(vlcInstance);
    }
}

void VLCPlayer::handleMediaEvents(const libvlc_event_t* event){
    qDebug()<<"called event manager -----------------";
    switch (event->type) {
    case libvlc_MediaPlayerPlaying:
        emit mediaStatusChanged(PlaybackState::Playing);
        break;
    case libvlc_MediaPlayerPaused:
        emit mediaStatusChanged(PlaybackState::Paused);
        break;
    case libvlc_MediaPlayerEndReached:
        emit mediaStatusChanged(PlaybackState::Ended);
        break;
    case libvlc_MediaPlayerBuffering:
        emit mediaStatusChanged(PlaybackState::Buffering);
        break;
    case libvlc_MediaPlayerMediaChanged:
        emit mediaStatusChanged(PlaybackState::Changed);
        break;
    case libvlc_MediaPlayerEncounteredError:
        emit mediaStatusChanged(PlaybackState::Error);
        break;
    default:
        emit mediaStatusChanged(PlaybackState::DontKnow);
        break;
    }
}

libvlc_media_t* VLCPlayer::create_media_from_buffer(libvlc_instance_t* vlcInstance, QBuffer* buffer) {
    if (!vlcInstance || !buffer)
        return nullptr;

    // Ensure the buffer is open
    if (!buffer->isOpen()) {
        buffer->open(QIODevice::ReadOnly);
    }

    // Allocate and initialize the context
    VlcBufferContext* context = new VlcBufferContext{ buffer };

    // Create the media with callbacks
    libvlc_media_t* media = libvlc_media_new_callbacks(
        vlcInstance,
        vlc_open_callback,
        vlc_read_callback,
        vlc_seek_callback,
        vlc_close_callback,
        context
        );

    if (!media) {
        qDebug() << "Failed to create media from buffer.";
        delete context;
        return nullptr;
    }
    return media;
}

void VLCPlayer::setupPositionChangedListener()
{
    //positionMonitoringThread = QThread::create(positionMonitor,nullptr);
    positionMonitoringThread  =  QThread::create([this](VLCPlayer* plyaer) {
        this->positionMonitor(plyaer); // Call your member function
    },this);
    positionMonitoringThread->start();
}

void VLCPlayer::positionMonitor(VLCPlayer* player)
{
    try {
        int sleepTimer = 1000;
        while(true){
            QThread::msleep(sleepTimer);
            if(isPlaying()){
                float currentPlaybackRate = libvlc_media_player_get_rate(player->mediaPlayer);
                sleepTimer = (int)(1000 / currentPlaybackRate);
                onPlaybackTimerTickOnVisibleSecond();
            }else {
                sleepTimer = 1000;
                //emit onPlaybackTimerTickOnVisibleSecond();
            }
        }
    }catch(std::exception &ex){
        qDebug()<<"exception ----------------------";
        qDebug()<<ex.what();
    }
}

void VLCPlayer::play()
{
    CHECK_MEDIAPLAYER_INITIALIZED(mediaPlayer);

    libvlc_media_player_play(mediaPlayer);

    emit playPausedStatusChanged(PlayPauseState::Play);
    // Play the media
}

void VLCPlayer::resume()
{
    CHECK_MEDIAPLAYER_INITIALIZED(mediaPlayer);
    libvlc_media_player_set_pause(mediaPlayer,0);
    emit playPausedStatusChanged(PlayPauseState::Play);
}

void VLCPlayer::pause()
{
    CHECK_MEDIAPLAYER_INITIALIZED(mediaPlayer);
    libvlc_media_player_set_pause(mediaPlayer,1);
    emit playPausedStatusChanged(PlayPauseState::Pause);
}

float VLCPlayer::playbackRate()
{
    return libvlc_media_player_get_rate(mediaPlayer);
}

void VLCPlayer::assignParentWidgetWinId(HWND id)
{
    if(!mediaPlayer || id == nullptr){
        qDebug()<<"media player or hwnd is null";
        return;
    }

    libvlc_media_player_set_hwnd(mediaPlayer, id);
}

void VLCPlayer::setMediaSource(QBuffer *videoBuffer)
{
    libvlc_media_t* media = create_media_from_buffer(vlcInstance, videoBuffer);
    // Set the media to the media player
    libvlc_media_player_set_media(mediaPlayer, media);

    // Release the media since the media player now holds a reference
    libvlc_media_release(media);
}

void VLCPlayer::changeSpeed(float speed)
{
    if (libvlc_media_player_set_rate(mediaPlayer, speed) == 0) {
        qDebug() << "Playback speed increased to:" << speed;
    }
}

bool VLCPlayer::isMuted()
{
    // if(!mediaPlayer){
    //     qDebug()<<"media player not initialized";
    //     return false;
    // }
    CHECK_MEDIAPLAYER_INITIALIZED(mediaPlayer);
    return libvlc_audio_get_mute(mediaPlayer) == 1;
}

void VLCPlayer::setMuted(bool mute)
{
    CHECK_MEDIAPLAYER_INITIALIZED(mediaPlayer);

    libvlc_audio_set_mute(mediaPlayer, mute?1:0);
}

float VLCPlayer::position()
{
    return libvlc_media_player_get_position(mediaPlayer);
}

int VLCPlayer::duration()
{
    libvlc_time_t duration = libvlc_media_player_get_length(mediaPlayer);
    return (int) duration / 1000;
}

bool VLCPlayer::isPlaying()
{
    PlayPauseState playingOrNot = playPauseState();
    return playingOrNot == PlayPauseState::Play;
}

void VLCPlayer::setVolume(int volume)
{
    CHECK_MEDIAPLAYER_INITIALIZED(mediaPlayer);
    if(volume >= 0 && volume <= 100){
        libvlc_audio_set_volume(mediaPlayer,volume);
    }else {
        qDebug()<<"incorrect volume value. value should be between 0..100";
    }
}

PlayPauseState VLCPlayer::playPauseState(){
    libvlc_state_t state = libvlc_media_player_get_state(mediaPlayer);
    switch(state){
        case libvlc_Playing:
            return PlayPauseState::Play;
        case libvlc_Paused:
            return PlayPauseState::Pause;
        default:
            return PlayPauseState::Pause;
    }
}

PlaybackState VLCPlayer::playbackState()
{
    libvlc_state_t state = libvlc_media_player_get_state(mediaPlayer);
    switch (state) {
    case libvlc_Opening:
        return PlaybackState::Opening;
    case libvlc_Buffering:
        return PlaybackState::Buffering;
    case libvlc_Playing:
        return PlaybackState::Playing;
    case libvlc_Paused:
        return PlaybackState::Paused;
    // case libvlc_Stopped: std::cout << "Media is stopped." << std::endl;
    //     break;
    case libvlc_Ended:
        return PlaybackState::Ended;
    case libvlc_Error:
        return PlaybackState::Error;
    default:
        return PlaybackState::DontKnow;
    }
}


void VLCPlayer::movePointerToPosition(int position){
    if (position > -1) {
        // Ensure the position is within the media duration
        //libvlc_time_t seekTime = static_cast<libvlc_time_t>(position);
        //libvlc_media_player_set_position(mediaPlayer, position / 100.0f);
        libvlc_media_player_set_time(mediaPlayer,position * 1000);
    }
}

void VLCPlayer::onPlaybackTimerTickOnVisibleSecond()
{
    //qDebug()<<"hello world........";
    float positionOfMedia = position();
    int durationOfMedia = duration();
    if(durationOfMedia > 0){
        timeOfVideo++;
        int playerTimeFromVlc = (int) floor(positionOfMedia * durationOfMedia);
        if((playerTimeFromVlc - timeOfVideo) > 1 || (playerTimeFromVlc - timeOfVideo) < -1){
            timeOfVideo = playerTimeFromVlc;
        }
        emit positionChanged(timeOfVideo);
    }
}


