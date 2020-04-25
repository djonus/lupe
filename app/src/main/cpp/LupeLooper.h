#ifndef LUPE_LOOPER_H
#define LUPE_LOOPER_H

#include <vector>
#include <oboe/Oboe.h>
#include <math.h>
#include "utils/logging.h"
#include "Loop.h"

class LupeLooper : public oboe::AudioStreamCallback {
public:

    LupeLooper(int sampleRate, int inputChannelCount);

    void record() {
        LOGD("Start recording");
        mIsRecording = true;
    }

    void stopRecording() {
        LOGD("Stop recording");
        mIsRecording = false;

        if (mTapeSize == 0 && mTape.size() > 0) {
            LOGD("Save save initial loop");
            Loop initialLoop = Loop("1", mTape);
            mLoops.push_back(initialLoop);
            mTapeSize = initialLoop.size();
            mSampleSize = mTapeSize * 840; //840 is least common multiplier for of tape size multipliers
        }
    }

    float sample();

    void saveCandidate() {
        if (mLoopCandidate.size() > 0) {
            LOGD("Save candidate loop");
            std::string id = std::to_string(mLoops.size());
            mLoopCandidate.setId(id);
            mLoops.push_back(mLoopCandidate);
        }
    }

    void dropLast() {
        if (mLoops.size() > 1) {
            LOGD("Deleted loop");
            mLoops.pop_back();
        } else if (mLoops.size() == 1) {
            LOGD("Delete last loop");
            mLoops.pop_back();
            mTapeSize = 0;
            mSampleCursor = 0;
            mSampleSize = 0;
            resetTape();
        }
    }

    Loop mLoopCandidate;
    std::vector<Loop> mLoops;
    int32_t mTapeSize = 0;
    int32_t mTapeCursor = 0;

    void setTapeSizeMultiplier(double multiplier) {
        if (mLoops.size() > 0) {
            mTapeSize = mLoops[0].size() * multiplier;
            mTapeCursor = mSampleCursor % mTapeSize;
        } else {
            mTapeSize = 0;
        }
    }

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:

    int mSampleRate;
    int mInputChannelCount;

    bool mIsRecording = false;
    bool mRecordingIsEmpty = true;

    std::vector<float> mTape;

    int32_t mSampleCursor = 0;
    int32_t mSampleSize = 0;

    void resetTape() {
        mTape.clear();
        mTapeCursor = 0;
        mRecordingIsEmpty = true;
    }

//    float delayFx(float originalSample) {
//        float sampleValue = originalSample;
//        int samplesPerGap = 18000;
//        int gaps = mRecordingCursor / samplesPerGap;
//
//        for (int j = 1; j < gaps; ++j) {
//            sampleValue += recorderdingSample(samplesPerGap * -j, 1.0F / powf(j + 0.3F, 3));
//        }
//        return sampleValue;
//    }
//
//    float recorderdingSample(int32_t samplesToSkip, float factor) {
//        int32_t cursor = mRecordingCursor + samplesToSkip;
//        if (cursor >= mRecording.size()) {
//            cursor = mRecording.size();
//        }
//        return mRecording[cursor] * factor;
//    }

};

#endif //LUPE_LOOPER_H
