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
            ->setCallback(this);

    oboe::Result result = builder.openManagedStream(mOutStream);
    if (result != oboe::Result::OK) {
        LOGE("Error opening managed stream: %s", oboe::convertToText(result));
    }

    int streamFramesPerBurst = mOutStream->getFramesPerBurst();
    LOGD("Stream framesPerBurst: %d", streamFramesPerBurst);

    mOutStream->setBufferSizeInFrames(streamFramesPerBurst * 2);
}

void LupeSoundEngine::start() {
    oboe::StreamState state = mOutStream->getState();

    LOGD("Engine state: %d]", state);
    if (state < oboe::StreamState::Starting || state > oboe::StreamState::Pausing) {
        LOGD("Engine start");
        oboe::Result result = mOutStream->start();
        if (result != oboe::Result::OK) {
            LOGE("Error starting out stream: %s", oboe::convertToText(result));
        }
    }
    mPlaySynth = true;
}

void LupeSoundEngine::stop() {
    LOGD("Engine state: %d]", mOutStream->getState());
    if (mOutStream->getState() < oboe::StreamState::Pausing) {
        LOGD("Engine pause");
        oboe::Result result = mOutStream->pause();
        if (result != oboe::Result::OK) {
            LOGE("Error pausing out stream: %s", oboe::convertToText(result));
        }
    }
    mPlaySynth = false;
}

void LupeSoundEngine::startPlayback(){
    LOGD("Engine start playback");
}

void LupeSoundEngine::stopPlayback(){
    LOGD("Engine stop playback");
}

void LupeSoundEngine::record(){
    LOGD("Engine record");
}

void LupeSoundEngine::stopRecord(){
    LOGD("Engine stop record");
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
