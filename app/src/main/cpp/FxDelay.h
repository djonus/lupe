#ifndef LUPE_FX_DELAY_H
#define LUPE_FX_DELAY_H

#include <queue>
#include <math.h>

class FxDelay {

public:

    FxDelay() = default;

    float sample(float sample) {
        if (mSampleBuffer.size() > mDelay) {
            // apply delay
            float fxSample = mSampleBuffer.front();
            mSampleBuffer.pop();

            mSampleBuffer.push((sample * mDecay) + (fxSample * mDecay));
            return sample + fxSample;
        } else {
            mSampleBuffer.push(sample);
            return sample;
        }
    }


    void setDelay(int sampleCount) {
        mDelay = sampleCount;
    }

    void setDecay(float ratio) {
        mDecay = ratio;
    }

private:
    std::queue<float> mSampleBuffer;
    int mDelay = 12000;
    float mDecay = 0.5F;
};

#endif //LUPE_FX_DELAY_H
