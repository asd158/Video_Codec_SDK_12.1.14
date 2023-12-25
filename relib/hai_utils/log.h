#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <fmt/format.h>
#include "Defines.h"

#ifdef ANDROID
#include <android/log.h>
#define TAG "HAI_NATIVE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__, nullptr)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__, nullptr)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__, nullptr)
#else
#define LOGI(...) printf("%s",__VA_ARGS__);
#define LOGW(...) printf("%s",__VA_ARGS__);
#define LOGE(...) printf("%s",__VA_ARGS__);
#endif
static auto startTime = std::chrono::system_clock::now();

template<typename... Args>
inline void LOG_ERROR(const std::string &p,
                      Args ... args) {
    auto end = std::chrono::system_clock::now();
    auto microSecond = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime).count();
    auto ss = microSecond % 1000000;
    auto s = microSecond / 1000000;
    auto h = s / 3600;
    s = s % 3600;
    auto m = s / 60;
    s = s % 60;
    std::string str = fmt::format("[{:>4}:{:02}:{:02}.{:<06}] [HAINATIVE_ERROR] {}\n",
                                  h,
                                  m,
                                  s,
                                  ss,
                                  p.c_str());
    str = fmt::format(str.c_str(),
                      args...);
    LOGE(str.c_str());
}

template<typename... Args>
inline void LOG_WARN(const std::string &p,
                     Args ... args) {
    auto end = std::chrono::system_clock::now();
    auto microSecond = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime).count();
    auto ss = microSecond % 1000000;
    auto s = microSecond / 1000000;
    auto h = s / 3600;
    s = s % 3600;
    auto m = s / 60;
    s = s % 60;
    std::string str = fmt::format("[{:>4}:{:02}:{:02}.{:<06}] [HAINATIVE_WARN] {}\n",
                                  h,
                                  m,
                                  s,
                                  ss,
                                  p.c_str());
    str = fmt::format(str.c_str(),
                      args...);
    LOGW(str.c_str());
}

template<typename... Args>
inline void LOG_INFO(const std::string &p,
                     Args ... args) {
    auto end = std::chrono::system_clock::now();
    auto microSecond = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime).count();
    auto ss = microSecond % 1000000;
    auto s = microSecond / 1000000;
    auto h = s / 3600;
    s = s % 3600;
    auto m = s / 60;
    s = s % 60;
    std::string str = fmt::format("[{:>4}:{:02}:{:02}.{:<06}] [HAINATIVE_INFO] {}\n",
                                  h,
                                  m,
                                  s,
                                  ss,
                                  p.c_str());
    str = fmt::format(str.c_str(),
                      args...);
    LOGI(str.c_str());
}