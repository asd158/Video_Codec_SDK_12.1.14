//*********************************************
// Created by weiya.gao 2020
//*********************************************


#include "StopWatch.h"

void StopWatch::Start()
{
    if (_isStart)
    {
        return;
    }
    _isStart = true;
    _start = std::chrono::steady_clock::now();
}

void StopWatch::Stop()
{
    if (!_isStart)
    {
        return;
    }
    _isStart = false;
    _end = std::chrono::steady_clock::now();
    _finalFS = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
}


long long StopWatch::EclipsedMillSecond() const
{
    if (!_isStart)
    {
        return _finalFS;
    }
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - _start).count();
}
