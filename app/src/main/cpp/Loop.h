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

    float sample(int32_t cursor) {
        mCursor = cursor % mLoop.size();
        return mLoop[mCursor];;
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
