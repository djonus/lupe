#ifndef LUPE_SINEPLAYER_H
#define LUPE_SINEPLAYER_H

#include <oboe/Oboe.h>
#include <math.h>

class OboeSinePlayer : public oboe::AudioStreamCallback {
public:

    OboeSinePlayer();

    void start();

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:
    oboe::ManagedStream outStream;
    // Stream params
    static int constexpr kChannelCount = 2;
    static int constexpr kSampleRate = 48000;
    // Wave params, these could be instance variables in order to modify at runtime
    static float constexpr kAmplitude = 0.5f;
    static float constexpr kFrequency = 440;
    static float constexpr kPI = M_PI;
    static float constexpr kTwoPi = kPI * 2;
    static double constexpr mPhaseIncrement = kFrequency * kTwoPi / (double) kSampleRate;
    // Keeps track of where the wave is
    float mPhase = 0.0;
};

#endif //LUPE_SINEPLAYER_H
