#include "LupeLooper.h"
#include <oboe/Oboe.h>
#include "utils/logging.h"

LupeLooper::LupeLooper(int sampleRate, int inputChannelCount) {
    LOGD("LupeLooper created: [sampleRate:%d]", sampleRate);
    mSampleRate = sampleRate;
    mInputChannelCount = inputChannelCount;
}

float LupeLooper::sample() {
    float sample = 0;
    for (int i = 0; i < mLoops.size(); ++i) {
        sample += mLoops[i].sample(mSampleCursor);
    }
    if (mLoops.size() > 0) {
        mSampleCursor++;
    }
    return sample;
}

oboe::DataCallbackResult
LupeLooper::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    auto *floatData = (float *) audioData;
    for (int i = 0; i < numFrames; ++i) {
        for (int j = 0; j < mInputChannelCount; j++) {
            if (mIsRecording) {
                mTape.push_back(floatData[i * mInputChannelCount + j]);
                mRecordingIsEmpty = false;
                mTapeCursor++;
            }
            else if (mSamplesPerTape > 0) { // mSamplesPerTape set on first stopRecording
                mTape.push_back(0);
                mTapeCursor++;
            }
            if (mSamplesPerTape > 0 && mTapeCursor >= mSamplesPerTape) {
                LOGD("Full loop");
                if (!mRecordingIsEmpty) {
                    LOGD("Update candidate");
                    mLoopCandidate = Loop("0", mTape);
                }

                resetTape();
            }
        }
    }
    return oboe::DataCallbackResult::Continue;
}
