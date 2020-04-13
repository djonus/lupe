#include <jni.h>
#include <oboe/Oboe.h>
#include "LupeSoundEngine.h"

#include "utils/logging.h"

extern "C" {
JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_setDefaultStreamValues(
        JNIEnv *env,
        jclass type,
        jint sampleRate,
        jint framesPerBurst) {

    LOGD("Update sample rate: %d -> %d", oboe::DefaultStreamValues::SampleRate, sampleRate);
    LOGD("Update frames per burst: %d -> %d", oboe::DefaultStreamValues::FramesPerBurst, framesPerBurst);

    oboe::DefaultStreamValues::SampleRate = (int32_t) sampleRate;
    oboe::DefaultStreamValues::FramesPerBurst = (int32_t) framesPerBurst;
}

/**
 * Creates the audio engine
 *
 * @return a pointer to the audio engine. This should be passed to other methods
 */
JNIEXPORT jlong JNICALL
Java_com_djonus_lupe_MainActivity_createEngine(
        JNIEnv *env,
        jclass /*unused*/) {
    // We use std::nothrow so `new` returns a nullptr if the engine creation fails
    LupeSoundEngine *engine = new(std::nothrow) LupeSoundEngine();
    return reinterpret_cast<jlong>(engine);
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_playSynth(
        JNIEnv *env,
        jclass type,
        jlong engineHandle,
        jdouble x,
        jdouble y) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->mLupeSynth.control(x, y);
    engine->start();
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_stopSynth(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->stop();
}

}

