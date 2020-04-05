#include <oboe/Oboe.h>
#include <math.h>
#include "OboeSinePlayer.h"

OboeSinePlayer::OboeSinePlayer() {
}

void OboeSinePlayer::start() {
    oboe::AudioStreamBuilder builder;
    // The builder set methods can be chained for convenience.
    builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setChannelCount(kChannelCount)
            ->setSampleRate(kSampleRate)
            ->setFormat(oboe::AudioFormat::Float)
            ->setCallback(this)
            ->openManagedStream(outStream);

    outStream->setBufferSizeInFrames(outStream->getFramesPerBurst() * 2);


    // Typically, start the stream after querying some stream information, as well as some input from the user
    outStream->requestStart();
}

oboe::DataCallbackResult
OboeSinePlayer::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    auto *floatData = (float *) audioData;
    for (int i = 0; i < numFrames; ++i) {
        float sampleValue = kAmplitude * sinf(mPhase);
        for (int j = 0; j < kChannelCount; j++) {
            floatData[i * kChannelCount + j] = sampleValue;
        }
        mPhase += mPhaseIncrement;
        if (mPhase >= kTwoPi) mPhase -= kTwoPi;
    }
    return oboe::DataCallbackResult::Continue;
}
