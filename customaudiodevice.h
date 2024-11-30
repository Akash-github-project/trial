#ifndef CUSTOM_AUDIO_DEVICE_H
#define CUSTOM_AUDIO_DEVICE_H

#include <QIODevice>
#include <soundtouch/SoundTouch.h>

class CustomAudioDevice : public QIODevice {
    Q_OBJECT

public:
    explicit CustomAudioDevice(QObject *parent = nullptr);
    void setInputDevice(QIODevice *device);
    void setPlaybackRate(float rate);

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    bool isSequential() const override;

private:
    soundtouch::SoundTouch soundTouch;
    QIODevice *inputDevice = nullptr;
};

#endif // CUSTOM_AUDIO_DEVICE_H
