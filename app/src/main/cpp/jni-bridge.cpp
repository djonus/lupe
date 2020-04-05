#include <jni.h>
#include <oboe/Oboe.h>
#include "OboeSinePlayer.h"


extern "C" {
JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_setDefaultStreamValues(
        JNIEnv *env,
        jclass type,
        jint sampleRate,
        jint framesPerBurst) {
    oboe::DefaultStreamValues::SampleRate = (int32_t) sampleRate;
    oboe::DefaultStreamValues::FramesPerBurst = (int32_t) framesPerBurst;
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_startSine(
        JNIEnv *env,
        jclass type) {
    OboeSinePlayer *player = new OboeSinePlayer();
    player->start();
}
}

