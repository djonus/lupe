#ifndef LUPE_SINEPLAYER_H
#define LUPE_SINEPLAYER_H

#include <oboe/Oboe.h>
#include <math.h>
#include "LupeSynth.h"
#include "LupeLooper.h"
#include "FxDelay.h"
#include <vector>

class LupeSoundEngine : public oboe::AudioStreamCallback {
public:

    LupeSoundEngine();

    void start();
    void stop();

    LupeSynth mLupeSynth;
    LupeLooper mLooper;

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:

    oboe::ManagedStream mOutStream;
    oboe::ManagedStream mInStream;

    // Stream params
    oboe::AudioFormat mFormat = oboe::AudioFormat::Float;
    static int32_t const mInputChannelCount = oboe::ChannelCount::Mono;
    static int32_t const mOutputChannelCount = oboe::ChannelCount::Stereo;

    FxDelay mSynthDelay;
};

#endif //LUPE_SINEPLAYER_H
