#ifndef ANDROID_LOGGING_H
#define ANDROID_LOGGING_H

#include <stdio.h>
#include <android/log.h>
#include <vector>

#define APP_NAME "Lupe"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, APP_NAME, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, APP_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, APP_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, APP_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, APP_NAME, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, APP_NAME, __VA_ARGS__)

#endif //ANDROID_LOGGING_H
