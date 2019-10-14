//
// Created by Irwin on 2019-10-12.
//

#include <android/log.h>

#ifndef WEBPLIB_LOGUTILS_H
#define WEBPLIB_LOGUTILS_H

#endif //WEBPLIB_LOGUTILS_H

#if 0
#define log_print_verbose(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define log_print_debug(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define log_print_info(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#else
#define log_print_verbose(...)
#define log_print_debug(...)
#define log_print_info(...)
#endif

#define log_print_warn(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define log_print_error(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define LOGV(...) log_print_verbose(__VA_ARGS__)
#define LOGD(...) log_print_debug(__VA_ARGS__)
#define LOGI(...) log_print_info(__VA_ARGS__)
#define LOGW(...) log_print_warn(__VA_ARGS__)
#define LOGE(...) log_print_error(__VA_ARGS__)