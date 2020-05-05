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
    engine->mLupeSynth.on();
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_stopSynth(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->mLupeSynth.off();
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_startPlayback(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->start();
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_stopPlayback(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->stop();
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_record(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->mLooper.record();
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_stopRecord(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->mLooper.stopRecording();
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_deleteLoop(
        JNIEnv *env,
        jclass type,
        jlong engineHandle,
        jint loopId) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->mLooper.deleteLoop(loopId);
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_saveCandidate(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->mLooper.saveCandidate();
}

JNIEXPORT void JNICALL
Java_com_djonus_lupe_MainActivity_setTapeSizeMultiplier(
        JNIEnv *env,
        jclass type,
        jlong engineHandle,
        jdouble multiplier) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);
    engine->mLooper.setTapeSizeMultiplier(multiplier);
}

JNIEXPORT jintArray JNICALL
Java_com_djonus_lupe_MainActivity_getLoopCursors(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);

    std::vector<Loop> loops = engine->mLooper.mLoops;
    std::vector<int32_t > cursorData;

    for (int i = 0; i < loops.size(); ++i) {
        cursorData.push_back(loops[i].size());
        cursorData.push_back(loops[i].cursor());
    }

    cursorData.push_back(engine->mLooper.mTapeSize);
    cursorData.push_back(engine->mLooper.mTapeCursor);

    jintArray converted  = env->NewIntArray(cursorData.size());
    env->SetIntArrayRegion(converted, 0, cursorData.size(), &cursorData[0]);

    return converted;
}

JNIEXPORT jintArray JNICALL
Java_com_djonus_lupe_MainActivity_getTrackDetails(
        JNIEnv *env,
        jclass type,
        jlong engineHandle) {

    LupeSoundEngine *engine = reinterpret_cast<LupeSoundEngine *>(engineHandle);

    std::vector<Loop> loops = engine->mLooper.mLoops;
    std::vector<int32_t > trackData;

    for (int i = 0; i < loops.size(); ++i) {
        int32_t id = loops[i].id();
        trackData.push_back(id);
    }

    trackData.push_back(-1); // candidate
    trackData.push_back(-2); // master

    jintArray converted  = env->NewIntArray(trackData.size());
    env->SetIntArrayRegion(converted, 0, trackData.size(), &trackData[0]);

    return converted;
}
}

