#ifndef LUPE_SYNTH_H
#define LUPE_SYNTH_H

#include <math.h>

class LupeSynth {
public:

    LupeSynth(double sampleRate);

    void control(int x, int y);
    float sample();

private:
    // Wave params, these could be instance variables in order to modify at runtime
    static float constexpr kAmplitude = 0.5f;
    static float constexpr kPI = M_PI;
    static float constexpr kTwoPi = kPI * 2;

    static int constexpr kMaxX = 20000;    //max frequency
    static int constexpr kMinX = 20;       //min frequency

    // Keeps track of where the wave is
    float mPhase = 0.0;

    double mPhaseIncrement;
    int mSampleRate;
    int mX = 0;
    int mY = 0;

    int normalize(int input) {
        return 0;
    }
};

#endif //LUPE_SYNTH_H
