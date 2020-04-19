#ifndef LUPE_LOOP_H
#define LUPE_LOOP_H

#include <cstdint>
#include <string>
#include "utils/logging.h"

class Loop {

public:

    Loop() = default;

    Loop(std::vector<float> loop) {
        mLoop = loop;
    };

    float sample() {
        float sample = mLoop[mCursor];
        mCursor++;
        if (mCursor >= mLoop.size()) {
            mCursor = 0;
        }
        return sample;
    }

    int32_t size() {
        return mLoop.size();
    }

    int32_t cursor() {
        return mCursor;
    }


private:
    std::vector<float> mLoop;
    int32_t mCursor = 0;
};


#endif //LUPE_LOOP_H
