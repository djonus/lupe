#include <oboe/Oboe.h>
#include <aaudio/AAudio.h>
#include "LupeSoundEngine.h"
#include "utils/logging.h"

LupeSoundEngine::LupeSoundEngine() :
mLupeSynth(oboe::DefaultStreamValues::SampleRate),
mLooper(oboe::DefaultStreamValues::SampleRate, mInputChannelCount) {
    LOGD("Hello from LupeSoundEngine");
    LOGD("Default framesPerBurst: %d", oboe::DefaultStreamValues::FramesPerBurst);

    int sampleRate = oboe::DefaultStreamValues::SampleRate;
    LOGD("Sample rate: %d", sampleRate);

    oboe::AudioStreamBuilder outBuilder;
    // The builder set methods can be chained for convenience.
    outBuilder.setFormat(mFormat)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSampleRate(sampleRate)
            ->setChannelCount(mOutputChannelCount)
            ->setCallback(this);

    oboe::Result result = outBuilder.openManagedStream(mOutStream);
    if (result != oboe::Result::OK) {
        LOGE("Error opening output stream: %s", oboe::convertToText(result));
    }

    int outFramesPerBurst = mOutStream->getFramesPerBurst();
    LOGD("Out stream framesPerBurst: %d", outFramesPerBurst);
    mOutStream->setBufferSizeInFrames(outFramesPerBurst * 2);


    // Setup input

    oboe::AudioStreamBuilder inBuilder;
    inBuilder.setFormat(mFormat)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setDirection(oboe::Direction::Input)
            ->setChannelCount(mInputChannelCount)
            ->setCallback(&mLooper);

    oboe::Result r = inBuilder.openManagedStream(mInStream);
    if (r != oboe::Result::OK) {
        LOGE("Error opening input stream: %s", oboe::convertToText(r));
    }

    int inFramesPerBurst = mInStream->getFramesPerBurst();
    LOGD("In stream framesPerBurst: %d", inFramesPerBurst);
    mInStream->setBufferSizeInFrames(inFramesPerBurst * 2);

    mSynthDelay.setDecay(0.6f);
    mSynthDelay.setDelay(16000);
}

void LupeSoundEngine::start() {
    // Out
    oboe::StreamState outState = mOutStream->getState();
    LOGD("Out stream state: %d]", outState);
    if (outState < oboe::StreamState::Starting || outState > oboe::StreamState::Pausing) {
        LOGD("Start out stream");
        oboe::Result result = mOutStream->start();
        if (result != oboe::Result::OK) {
            LOGE("Error starting out stream: %s", oboe::convertToText(result));
        }
    }

    // In
    oboe::StreamState inState = mInStream->getState();
    LOGD("In stream state: %d]", inState);
    if (inState < oboe::StreamState::Starting || outState > oboe::StreamState::Pausing) {
        LOGD("Start in stream");
        oboe::Result result = mInStream->start();
        if (result != oboe::Result::OK) {
            LOGE("Error starting in stream: %s", oboe::convertToText(result));
        }
    }
}

void LupeSoundEngine::stop() {
    // Out
    LOGD("Out stream state: %d]", mOutStream->getState());
    if (mOutStream->getState() < oboe::StreamState::Pausing) {
        LOGD("Stop out stream");
        oboe::Result result = mOutStream->stop();
        if (result != oboe::Result::OK) {
            LOGE("Error stopping out stream: %s", oboe::convertToText(result));
        }
    }

    // In
    LOGD("In stream state: %d]", mInStream->getState());
    if (mInStream->getState() < oboe::StreamState::Pausing) {
        LOGD("Stop in stream");
        oboe::Result result = mInStream->stop();
        if (result != oboe::Result::OK) {
            LOGE("Error stopping in stream: %s", oboe::convertToText(result));
        }
    }
}

oboe::DataCallbackResult
LupeSoundEngine::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    auto *floatData = (float *) audioData;
    for (int i = 0; i < numFrames; ++i) {
        float sampleValue = mSynthDelay.sample(mLupeSynth.sample()) + mLooper.sample();
        for (int j = 0; j < mOutputChannelCount; j++) {
            floatData[i * mOutputChannelCount + j] = sampleValue;
        }
    }
    return oboe::DataCallbackResult::Continue;
}
