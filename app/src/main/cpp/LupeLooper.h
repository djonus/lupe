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
            add(mTape);
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
            clear();
        }
    }

    Loop mLoopCandidate;
    std::vector<Loop> mLoops;
    int32_t mTapeSize = 0;
    int32_t mTapeCursor = 0;

    void setTapeSizeMultiplier(double multiplier) {
        if (mLoops.size() > 0) {
            mTapeSize = mLoops[0].size() * multiplier;
            mTapeCursor = mCommonLoopCursor % mTapeSize;
        } else {
            mTapeSize = 0;
        }
    }

    void clear() {
        mIsRecording = false;
        mCommonLoopCursor = 0;
        mCommonLoopLength = 0;
        resetTape();
        mTapeSize = 0;
        mLoopCandidate = Loop(-1, mTape);
        mLoops.clear();
    }

    void add(std::vector<float> samples) {
        if (mLoops.size() == 0) {
            mTapeSize = samples.size();
            mCommonLoopLength = mTapeSize * 840; //840 is least common multiplier for of tape size multipliers
        }

        mLoops.push_back(Loop(mLoops.size(), samples));
    }

    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

private:

    int mSampleRate;
    int mInputChannelCount;

    bool mIsRecording = false;

    std::vector<float> mTape;

    int32_t mCommonLoopCursor = 0;
    int32_t mCommonLoopLength = 0;

    void resetTape() {
        mTape.clear();
        mTapeCursor = 0;
    }
};

#endif //LUPE_LOOPER_H
