//*********************************************
// Created by weiya.gao 2020
//*********************************************

#pragma once
#include <chrono>

class StopWatch
{
public:
    void Start();
    void Stop();
    [[nodiscard]] long long EclipsedMillSecond() const;
private:
    bool _isStart{false};
    long long _finalFS{-1};
    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _end;
};
