#ifndef LUPE_SINTH_H
#define LUPE_SINTH_H

#include "LupeSynth.h"
#include <oboe/Oboe.h>
#include <cmath>
#include "utils/logging.h"

LupeSynth::LupeSynth(double sampleRate) {
    LOGD("Hello from LupeSynth: [sampleRate:%d]", sampleRate);
    mSampleRate = sampleRate;
}

void LupeSynth::control(int x, int y) {
    mX = x;
    mY = y;

    mPhaseIncrement = mX * kTwoPi / (double) mSampleRate;
}

float LupeSynth::sample() {
    float result = kAmplitude * sinf(mPhase);
    mPhase += mPhaseIncrement;
    if (mPhase >= kTwoPi) mPhase -= kTwoPi;
    return result;
}

#endif //LUPE_SINTH_H
