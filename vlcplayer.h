#ifndef VLCPLAYER_H
#define VLCPLAYER_H

#include "videowidget.h"
#include <QObject>
#include <QBuffer>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <windows.h>
#include <vlc/vlc.h>

class VideoWidget;

enum PlayPauseState {
    Play,
    Pause
};

enum PlaybackState {
    Opening,
    Buffering,
    Changed,
    Playing,
    Paused,
    Ended,
    Error,
    DontKnow
};

struct VlcBufferContext {
    QBuffer* buffer;
};

struct VlcVideoData {
    VideoWidget* videoWidget; // Pointer to your custom VideoWidget
    QImage frame;             // Image to store the current video frame
    QMutex mutex;             // Mutex to protect access to the frame
};

#define CHECK_MEDIAPLAYER_INITIALIZED(mediaPlayer) \
if (!(mediaPlayer)) { \
        qDebug() << "Media player not initialized"; \
}


extern "C" {
void* lock_callback(void* opaque, void** planes);
void unlock_callback(void* opaque, void* picture, void* const* planes);
void display_callback(void* opaque, void* picture);
// Declare your callbacks here or include Callbacks.cpp
int vlc_open_callback(void* data, void** p_datap, uint64_t* sizep);
ssize_t vlc_read_callback(void* data, unsigned char* buf, size_t len);
int vlc_seek_callback(void* data, uint64_t offset);
void vlc_close_callback(void* data);
void media_event_callback(const libvlc_event_t* event, void* data);
}

class VLCPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VLCPlayer(QObject *parent = nullptr);
    explicit VLCPlayer(QObject *parent,VideoWidget* videoWidget);
    // general value based variables or constants
private:
    const char * const vlc_args[5] = {
        "--no-video-title-show", // Don't show the video title
        "--no-xlib" ,             // Disable Xlib (irrelevant for Windows but harmless),
        "--avcodec-hw=dxva2",
        "--vout=d3d11",
        "--no-sub-autodetect-file"
        "--quite",
    };
    int videoStatus;
    int timeOfVideo = 0;

    // instance type heavy objects maybe heap allocated
private:
    // libVLC instances
    libvlc_instance_t *vlcInstance;
    libvlc_media_player_t *mediaPlayer;
    libvlc_media_t *m;
    libvlc_event_manager_t* media_event_manager;
    QThread *positionMonitoringThread;
    QTimer *positionCheckTimer;
    VlcVideoData* videoData;

private:
    libvlc_media_t *create_media_from_buffer(libvlc_instance_t *vlcInstance, QBuffer *buffer);  // done
    void setupPositionChangedListener();
    //void positionMonitor(libvlc_media_player_t* mediaPlayer);
    void positionMonitor(VLCPlayer* player);

public:
    void play(); //done
    void resume(); //done
    void pause();//done
    float playbackRate();
    void assignParentWidgetWinId(HWND id); //done
    void setMediaSource(QBuffer* videoBuffer); // done
    void changeSpeed(float speed);
    bool isMuted(); //done
    int getVolume();
    void setMuted(bool mute); //done
    float position(); // done
    int duration(); // done
    bool isPlaying(); // done
    void setVolume(int volume); // done
    void handleMediaEvents(const libvlc_event_t *event);
    PlaybackState playbackState(); // done
    void movePointerToPosition(int position);
    ~VLCPlayer();
    PlayPauseState playPauseState();
private slots:
    void onPlaybackTimerTickOnVisibleSecond();


signals:
    void positionChanged(int second); // done
    void mediaStatusChanged(PlaybackState playbackState); //
    void playPausedStatusChanged(PlayPauseState playPaused);
    void frameReady(const QImage& frame);

};

#endif // VLCPLAYER_H
