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

        if (mSamplesPerTape == 0 && mTape.size() > 0) {
            LOGD("Save save initial loop");
            Loop initialLoop = Loop(mTape);
            mLoops.push_back(initialLoop);
            mSamplesPerTape = initialLoop.size();
            resetTape();
        }
    }

    float sample();

    void saveCandidate() {
        if (mLoopCandidate.size() > 0) {
            LOGD("Save candidate loop");
            mLoops.push_back(mLoopCandidate);
        }
    }

    void dropLast() {
        if (mLoops.size() > 1) {
            LOGD("Deleted loop");
            mLoops.pop_back();
        } else {
            LOGD("Delete last loop");
            mLoops.pop_back();
            mSamplesPerTape = 0;
            resetTape();
        }
    }

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:

    int mSampleRate;
    int mInputChannelCount;

    bool mIsRecording = false;
    bool mRecordingIsEmpty = true;

    int32_t mSamplesPerTape = 0;
    std::vector<float> mTape;
    int32_t mTapeCursor = 0;
    Loop mLoopCandidate;
    std::vector<Loop> mLoops;

    int32_t mSampleCursor = 0;

    void resetTape() {
        mTape.clear();
        mTapeCursor = 0;
        mSampleCursor = 0;
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
