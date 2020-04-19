#ifndef LUPE_SINEPLAYER_H
#define LUPE_SINEPLAYER_H

#include <oboe/Oboe.h>
#include <math.h>
#include "LupeSynth.h"
#include <vector>

class LupeSoundEngine : public oboe::AudioStreamCallback {
public:

    LupeSoundEngine();

    void start();
    void stop();

    void record();
    void stopRecord();
    void startPlayback();
    void stopPlayback();

    LupeSynth mLupeSynth;

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:

    oboe::ManagedStream mOutStream;

    bool mIsRecording = false;
    bool mIsPlayBackOn = false;
    std::vector<float> mRecording;
    int32_t mRecordingCursor = 0;
    // Stream params
    oboe::AudioFormat mFormat = oboe::AudioFormat::Float;
    int32_t mInputChannelCount = oboe::ChannelCount::Mono;
    int32_t mOutputChannelCount = oboe::ChannelCount::Stereo;

    float recorderdingSample(int32_t samplesToSkip, float factor) {
        int32_t cursor = mRecordingCursor + samplesToSkip;
        if (cursor >= mRecording.size()) {
            cursor = mRecording.size();
        }
        return mRecording[cursor] * factor;
    }
};

#endif //LUPE_SINEPLAYER_H
