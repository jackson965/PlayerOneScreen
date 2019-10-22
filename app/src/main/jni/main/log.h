#ifndef LOG_H
#define LOG_H

#include "../sdl/include/SDL_log.h"
#include <android/log.h>

#define  LOG_TAG    "JAI"
#define LOGV(FORMAT,...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG,FORMAT, ##__VA_ARGS__)
#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,FORMAT, ##__VA_ARGS__)
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,FORMAT,##__VA_ARGS__);
#define LOGW(FORMAT,...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,FORMAT, ##__VA_ARGS__)
#define LOGC(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,FORMAT, ##__VA_ARGS__)


//#define LOGV(...) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
//#define LOGD(...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
//#define LOGI(...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
//#define LOGW(...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
//#define LOGE(...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
//#define LOGC(...) SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

#endif
