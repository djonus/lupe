#ifndef LUPE_SINEPLAYER_H
#define LUPE_SINEPLAYER_H

#include <oboe/Oboe.h>
#include <math.h>
#include "LupeSynth.h"

class LupeSoundEngine : public oboe::AudioStreamCallback {
public:

    LupeSoundEngine();

    void playSynth(int x, int y);
    void stopSynth();

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:
    LupeSynth mLupeSynth;
    oboe::ManagedStream mOutStream;
    bool mPlaySynth = false;
    // Stream params
    static int constexpr kChannelCount = 1;
};

#endif //LUPE_SINEPLAYER_H
