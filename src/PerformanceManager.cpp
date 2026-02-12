#include "../include/PerformanceManager.h"
#include <iostream>
#include <cassert>

using us = std::chrono::microseconds;
using timePoint = std::chrono::steady_clock::time_point;

// ----------------------------------------------------
// Helpers
// ----------------------------------------------------

static us calculateTimeDifference(const timePoint& start,
                                  const timePoint& end)
{
    return std::chrono::duration_cast<us>(end - start);
}

// ----------------------------------------------------
// Public API functions
// ----------------------------------------------------

PerformanceManager::PerformanceManager()
{
    startSystemTimer();
}

PerformanceManager::~PerformanceManager()
{
    if (systemTimerRunning)
        stopSystemTimer();
}

void PerformanceManager::startFrameTimer()
{
    assert(!frameTimerRunning);
    startFrameTime = std::chrono::steady_clock::now();
    frameTimerRunning = true;
}

us PerformanceManager::stopFrameTimer()
{
    assert(frameTimerRunning);

    timePoint end = std::chrono::steady_clock::now();
    us duration = calculateTimeDifference(startFrameTime, end);

    frameTimerRunning = false;
    frameCounter++;

    return duration;
}

void PerformanceManager::startFFTTimer()
{
    assert(!fftTimerRunning);
    startFFTTime = std::chrono::steady_clock::now();
    fftTimerRunning = true;
}

us PerformanceManager::stopFFTTimer()
{
    assert(fftTimerRunning);

    timePoint end = std::chrono::steady_clock::now();
    us duration = calculateTimeDifference(startFFTTime, end);

    fftTimerRunning = false;
    fftCounter++;

    return duration;
}

void PerformanceManager::startRenderTimer()
{
    assert(!renderTimerRunning);
    startRenderTime = std::chrono::steady_clock::now();
    renderTimerRunning = true;
}

us PerformanceManager::stopRenderTimer()
{
    assert(renderTimerRunning);

    timePoint end = std::chrono::steady_clock::now();
    us duration = calculateTimeDifference(startRenderTime, end);

    renderTimerRunning = false;
    renderCounter++;

    return duration;
}

void PerformanceManager::writePerformanceData()     //  Writes to std::cout for now, will write to JSON eventually.
{
    if (systemTimerRunning)
        systemRunTime = stopSystemTimer();

    std::cout << "----------------------------------------------------\n";
    std::cout << "             Performance Summary\n";
    std::cout << "----------------------------------------------------\n";
    std::cout << "System Runtime: " << systemRunTime.count() << " us\n";
    std::cout << "Frames:         " << frameCounter  << "\n";
    std::cout << "FFTs:           " << fftCounter    << "\n";
    std::cout << "Renders:        " << renderCounter << "\n";
}

// ----------------------------------------------------
// Private Class functions
// ----------------------------------------------------

void PerformanceManager::startSystemTimer()
{
    systemStartTime = std::chrono::steady_clock::now();
    systemTimerRunning = true;
}

us PerformanceManager::stopSystemTimer()
{
    assert(systemTimerRunning);

    timePoint end = std::chrono::steady_clock::now();
    us duration = calculateTimeDifference(systemStartTime, end);

    systemTimerRunning = false;

    return duration;
}
