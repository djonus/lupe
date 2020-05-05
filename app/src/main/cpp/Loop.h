#ifndef LUPE_LOOP_H
#define LUPE_LOOP_H

#include <cstdint>
#include <string>
#include "utils/logging.h"

class Loop {

public:

    Loop() = default;

    Loop(int32_t id, std::vector<float> loop) {
        mId = id;
        mLoop = loop;
    };

    float sample(int32_t cursor) {
        mCursor = cursor % mLoop.size();
        return mLoop[mCursor];
    }

    int32_t size() {
        return mLoop.size();
    }

    int32_t cursor() {
        return mCursor;
    }

    int32_t id() {
        return mId;
    }

    void setId(int32_t id) {
        mId = id;
    }


private:
    std::vector<float> mLoop;
    int32_t mCursor = 0;
    int32_t mId;
};


#endif //LUPE_LOOP_H
