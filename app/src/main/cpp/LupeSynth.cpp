#include "LupeSynth.h"
#include <oboe/Oboe.h>
#include "utils/logging.h"

LupeSynth::LupeSynth(int sampleRate) {
    LOGD("LupeSynth created: [sampleRate:%d]", sampleRate);
    mSampleRate = sampleRate;
}

void LupeSynth::control(double x, double y) {
    LOGD("LupeSynth controlled: [x:%f] [y:%f]", x, y);

    mX = x;
    mY = y;

    mAmplitude = (float) mY;
    mPhaseIncrement = mX * kTwoPi / (double) mSampleRate;
}

float LupeSynth::sample() {
    if (mIsOn) {
        float result = mAmplitude * sinf(mPhase);
        mPhase += mPhaseIncrement;
        if (mPhase >= kTwoPi) mPhase -= kTwoPi;
        return result;
    } else {
        return 0;
    }
}
