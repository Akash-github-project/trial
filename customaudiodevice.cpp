#include "customaudiodevice.h"

CustomAudioDevice::CustomAudioDevice(QObject *parent) : QIODevice(parent) {
    soundTouch.setSampleRate(44100);  // Default sample rate (adjust as needed)
    soundTouch.setChannels(2);       // Stereo audio
}

void CustomAudioDevice::setInputDevice(QIODevice *device) {
    inputDevice = device;
    if (inputDevice) {
        inputDevice->open(QIODevice::ReadOnly);
    }
}

void CustomAudioDevice::setPlaybackRate(float rate) {
    soundTouch.setTempo(rate);  // Adjust tempo while preserving pitch
}

qint64 CustomAudioDevice::readData(char *data, qint64 maxlen) {
    if (!inputDevice) return 0;

    // Read audio data from the input device
    QByteArray buffer = inputDevice->read(maxlen);
    if (buffer.isEmpty()) return 0;

    // Feed data into SoundTouch for processing
    soundTouch.putSamples(reinterpret_cast<const float *>(buffer.data()),
                          buffer.size() / sizeof(float) / soundTouch.numChannels());

    // Retrieve processed audio samples
    QByteArray outputBuffer;
    int numSamples = soundTouch.receiveSamples(reinterpret_cast<float *>(data),
                                               maxlen / sizeof(float) / soundTouch.numChannels());
    outputBuffer.resize(numSamples * sizeof(float) * soundTouch.numChannels());

    memcpy(data, outputBuffer.constData(), outputBuffer.size());
    return outputBuffer.size();
}

qint64 CustomAudioDevice::writeData(const char *data, qint64 len) {
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0;  // Write operation is not supported
}

bool CustomAudioDevice::isSequential() const {
    return true;
}
