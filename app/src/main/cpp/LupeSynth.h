#ifndef LUPE_SYNTH_H
#define LUPE_SYNTH_H

#include <math.h>

class LupeSynth {
public:

    LupeSynth(int sampleRate);

    void control(double x, double y);
    float sample();

private:
    // Wave params, these could be instance variables in order to modify at runtime
    static float constexpr kPI = M_PI;
    static float constexpr kTwoPi = kPI * 2;

    static int constexpr kMaxX = 20000;    //max frequency
    static int constexpr kMinX = 20;       //min frequency

    static int constexpr kMaxY = 1;    //max amplitude
    static int constexpr kMinY = 0;    //min amplitude

    // Keeps track of where the wave is
    float mPhase = 0.0;

    float mAmplitude;
    double mPhaseIncrement;
    int mSampleRate;
    double mX = 0;
    double mY = 0;

    int normalize(int input) {
        return 0;
    }
};

#endif //LUPE_SYNTH_H
