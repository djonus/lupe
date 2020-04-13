#ifndef LUPE_SINTH_H
#define LUPE_SINTH_H

#include "LupeSynth.h"
#include <oboe/Oboe.h>
#include <cmath>
#include "utils/logging.h"

LupeSynth::LupeSynth(int sampleRate) {
    LOGD("LupeSynth created: [sampleRate:%d]", sampleRate);
    mSampleRate = sampleRate;
}

void LupeSynth::control(double x, double y) {
    LOGD("LupeSynth controlled: [state:%d] [x:%f] [y:%f]", x, y);

    mX = x;
    mY = y;

    mAmplitude = (float) mY;
    mPhaseIncrement = mX * kTwoPi / (double) mSampleRate;
}

float LupeSynth::sample() {
    float result = mAmplitude * sinf(mPhase);
    mPhase += mPhaseIncrement;
    if (mPhase >= kTwoPi) mPhase -= kTwoPi;
    return result;
}

#endif //LUPE_SINTH_H
