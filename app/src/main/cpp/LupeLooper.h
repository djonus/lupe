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
            Loop initialLoop = Loop(0, mTape);
            mLoops.push_back(initialLoop);
            mTapeSize = initialLoop.size();
            mSampleSize = mTapeSize * 840; //840 is least common multiplier for of tape size multipliers
        }
    }

    float sample();

    void saveCandidate() {
        if (mLoopCandidate.size() > 0) {
            LOGD("Save candidate loop");
            mLoopCandidate.setId(mLoops.size());
            mLoops.push_back(mLoopCandidate);
        }
    }

    void deleteLoop(int32_t loopId) {
        if (mLoops.size() > 1) {
            for (int i = 0; i < mLoops.size(); ++i) {
                if (mLoops[i].id() == loopId) {
                    LOGD("Delete loop with id: %d", loopId);
                    mLoops.erase(mLoops.cbegin() + i);
                    return;
                }
            }
            LOGD("Can not delete loop with id: %d", loopId);
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
};

#endif //LUPE_LOOPER_H
