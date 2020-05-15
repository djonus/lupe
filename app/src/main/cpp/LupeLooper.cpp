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
        sample += mLoops[i].sample(mCommonLoopCursor);
    }
    if (mLoops.size() > 0) {
        mCommonLoopCursor++;
    }
    if (mCommonLoopCursor > mCommonLoopLength) {
        mCommonLoopCursor = 0;
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
                mTapeCursor++;
            }
            else if (mTapeSize > 0) { // mTapeSize set on first stopRecording
                mTape.push_back(0);
                mTapeCursor++;
            }
            if (mTapeSize > 0 && mTapeCursor >= mTapeSize) {
                LOGD("Full loop");
                if (mTape.size() > 0) {
                    LOGD("Update candidate");
                    mLoopCandidate = Loop(-1, mTape);
                }

                resetTape();
            }
        }
    }
    return oboe::DataCallbackResult::Continue;
}
