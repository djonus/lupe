#include <oboe/Oboe.h>
#include "LupeSoundEngine.h"
#include "utils/logging.h"

LupeSoundEngine::LupeSoundEngine() : mLupeSynth(oboe::DefaultStreamValues::SampleRate) {
    LOGD("Hello from LupeSoundEngine");
    LOGD("Default frames per burst: %d", oboe::DefaultStreamValues::FramesPerBurst);

    int sampleRate = oboe::DefaultStreamValues::SampleRate;
    LOGD("Sample rate: %d", sampleRate);

    oboe::AudioStreamBuilder builder;
    // The builder set methods can be chained for convenience.
    builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setChannelCount(kChannelCount)
            ->setSampleRate(sampleRate)
            ->setFormat(oboe::AudioFormat::Float)
            ->setCallback(this)
            ->openManagedStream(mOutStream);

    int streamFramesPerBurst = mOutStream->getFramesPerBurst();
    LOGD("Stream framesPerBurst: %d", streamFramesPerBurst);

    mOutStream->setBufferSizeInFrames(streamFramesPerBurst * 2);
}

void LupeSoundEngine::playSynth(int x, int y) {
    oboe::StreamState state = mOutStream->getState();

    LOGD("Play synth: [state:%d] [x:%d]", state, x);
    mLupeSynth.control(x, y);

    if (state < oboe::StreamState::Starting || state > oboe::StreamState::Pausing) {
        mOutStream->start();
    }
    mPlaySynth = true;
}

void LupeSoundEngine::stopSynth() {
    LOGD("Stop synth: [state%d]", mOutStream->getState());
    if (mOutStream->getState() < oboe::StreamState::Pausing) {
        mOutStream->pause();
    }
    mPlaySynth = false;
}

oboe::DataCallbackResult
LupeSoundEngine::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    auto *floatData = (float *) audioData;
    for (int i = 0; i < numFrames; ++i) {
        float sampleValue = mLupeSynth.sample();
        for (int j = 0; j < kChannelCount; j++) {
            floatData[i * kChannelCount + j] = sampleValue;
        }
    }
    return oboe::DataCallbackResult::Continue;
}
