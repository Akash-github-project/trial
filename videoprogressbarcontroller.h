#ifndef VIDEOPROGRESSBARCONTROLLER_H
#define VIDEOPROGRESSBARCONTROLLER_H
#include "QtCore/qtypes.h"
#include <qslider.h>
#include <qtimer.h>

class VideoProgressBarController : public QObject
{

    Q_OBJECT

public:
    VideoProgressBarController();
    VideoProgressBarController(QSlider *slider,int videoCount);

    ~VideoProgressBarController();

    void resetCurrentIndex();
    void setCurrentIndex(int index);
    //
    void setSliderTime(qint64 timeIn);
    void resetSliderTime();
    //
    void setSliderMaxLimit(qint64 maxDuration);
    void resetSlider();
    void moveSlider(int moveTo);
    int getSliderPosition();
    int getValue();


    //debounceing
    /*
    * call this fuction only when user is slding the seekbar
    * this is not intended to be called every time the seekbar moves like
    * when it moves in case of normal video playback
    */
    void setupTimer(QObject *parent);
    QSlider *horizontalSlider_Duration;
    void updateMaxLimit(qint64 maxDuration, int videoCount);
    void jumpInstantly(QObject *parent);
private slots:
    void onSeekbarDebounceTimerEnd();
signals:
    void onSeekbarStopedSliding();

private:
    QTimer *debounceTimer = nullptr;
    int currentIndex = 0;
    int videosCount = 0;
    qint64 videoCompleteDuration = 0;
    qint64 sliderTime = -1;
    bool jumpedJustNow = false;

};

#endif // VIDEOPROGRESSBARCONTROLLER_H
