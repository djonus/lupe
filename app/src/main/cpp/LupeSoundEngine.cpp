#include <oboe/Oboe.h>
#include <aaudio/AAudio.h>
#include "LupeSoundEngine.h"
#include "utils/logging.h"

LupeSoundEngine::LupeSoundEngine() : mLupeSynth(oboe::DefaultStreamValues::SampleRate) {
    LOGD("Hello from LupeSoundEngine");
    LOGD("Default frames per burst: %d", oboe::DefaultStreamValues::FramesPerBurst);

    int sampleRate = oboe::DefaultStreamValues::SampleRate;
    LOGD("Sample rate: %d", sampleRate);

    oboe::AudioStreamBuilder builder;
    // The builder set methods can be chained for convenience.
    builder.setFormat(mFormat)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSampleRate(sampleRate)
            ->setChannelCount(mOutputChannelCount)
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
}

void LupeSoundEngine::startPlayback() {
    LOGD("Engine start playback");
    mIsPlayBackOn = true;
}

void LupeSoundEngine::stopPlayback() {
    LOGD("Engine stop playback");
    mIsPlayBackOn = false;
}

void LupeSoundEngine::record() {
    LOGD("Engine record");

    oboe::AudioStreamBuilder builder;
    builder.setFormat(mFormat)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setDirection(oboe::Direction::Input)
            ->setChannelCount(mInputChannelCount)
            ->setCallback(nullptr);

    oboe::AudioStream *stream;
    oboe::Result r = builder.openStream(&stream);
    if (r != oboe::Result::OK) {
        LOGE("Error opening recording stream: %s", oboe::convertToText(r));
    }

    r = stream->requestStart();
    if (r != oboe::Result::OK) {
        LOGE("Error starting recording stream: %s", oboe::convertToText(r));
    }

    constexpr  int kMillisecondsToRecord = 2;
    const int32_t bufferSize = (int32_t)(kMillisecondsToRecord * (stream->getSampleRate() / kMillisecondsToRecord));
    std::unique_ptr<float[]> mInputBuffer = std::make_unique<float[]>(bufferSize);

    constexpr int64_t kNanosecondsInMillisecond = 1000000;
    constexpr int64_t kTimeoutValue = 3 * kNanosecondsInMillisecond;

    int framesRead = 0;
    do {
        auto result = stream->read(mInputBuffer.get(), stream->getBufferSizeInFrames(), 0);
        if (result != oboe::Result::OK) break;
        framesRead = result.value();
    } while (framesRead != 0);

    mRecording.clear();
    mIsRecording = true;
    mRecordingCursor = 0;
    int16_t burstsToRead = 3000;
    int16_t burstI = 0;
    while (mIsRecording) {
        auto result = stream->read(mInputBuffer.get(), bufferSize, kTimeoutValue);
        if (result == oboe::Result::OK) {
            const int32_t actualFrames = result.value();
            LOGD("Red %d frames", actualFrames);
            burstI++;
            if (burstsToRead <= burstI) {
                mIsRecording = false;
            }
            for (int i = 0; i < actualFrames ; ++i) {
                mRecording.push_back(mInputBuffer.get()[i]);
            }
        } else {
            LOGE("Error reading recording stream: %s", oboe::convertToText(result.error()));
            mIsRecording = false;
        }
    }
    stream->close();
}

void LupeSoundEngine::stopRecord(){
    LOGD("Engine stop record");
    mIsRecording = false;
}

oboe::DataCallbackResult
LupeSoundEngine::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    auto *floatData = (float *) audioData;
    for (int i = 0; i < numFrames; ++i) {
        float sampleValue;
        if (mIsPlayBackOn) {
            mRecordingCursor++;
            if (mRecordingCursor == mRecording.size()) {
                mRecordingCursor = 0;
            }
            sampleValue = mRecording[mRecordingCursor];

            int samplesPerGap = 18000;
            int gaps = mRecordingCursor / samplesPerGap;

            for (int j = 1; j < gaps; ++j) {
                sampleValue += recorderdingSample(samplesPerGap * -j, 1.0F / powf(j + 0.3F, 3));
            }
        } else {
            sampleValue = mLupeSynth.sample();
        }
        for (int j = 0; j < mOutputChannelCount; j++) {
            floatData[i * mOutputChannelCount + j] = sampleValue;
        }
    }
    return oboe::DataCallbackResult::Continue;
}
