#ifndef LUPE_LOOP_H
#define LUPE_LOOP_H

#include <cstdint>
#include <string>
#include "utils/logging.h"

class Loop {

public:

    Loop() = default;

    Loop(std::string id, std::vector<float> loop) {
        mId = id;
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

    std::string id() {
        mId;
    }

    void setId(std::string id) {
        mId = id;
    }


private:
    std::vector<float> mLoop;
    int32_t mCursor = 0;
    std::string mId;
};


#endif //LUPE_LOOP_H
