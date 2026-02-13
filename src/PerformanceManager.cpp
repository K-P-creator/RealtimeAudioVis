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
// Benchmark API Functions
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

    const timePoint end = std::chrono::steady_clock::now();
    const us duration = calculateTimeDifference(startFrameTime, end);

    frameTimerRunning = false;
    frameCounter++;
    mostRecentFrameTime_us = duration.count();
    frameTimeAccumulator_us += duration.count();

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

    const timePoint end = std::chrono::steady_clock::now();
    const us duration = calculateTimeDifference(startFFTTime, end);

    fftTimerRunning = false;
    fftCounter++;
    mostRecentFFTTime_us = duration.count();
    fftTimeAccumulator_us += duration.count();

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

    const timePoint end = std::chrono::steady_clock::now();
    const us duration = calculateTimeDifference(startRenderTime, end);

    renderTimerRunning = false;
    renderCounter++;
    mostRecentRenderTime_us = duration.count();
    renderTimeAccumulator_us += duration.count();

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
// Performance Overlay API functions
// ----------------------------------------------------


us PerformanceManager::getCurrentFFTTime() const
{
    return std::chrono::microseconds (mostRecentFFTTime_us);
}

us PerformanceManager::getCurrentRenderTime() const
{
    return std::chrono::microseconds(mostRecentRenderTime_us);
}
us PerformanceManager::getCurrentFrameTime() const
{
    return std::chrono::microseconds (mostRecentFrameTime_us);
}
us PerformanceManager::getAverageFFTTime() const
{
    if (fftCounter == 0) return std::chrono::microseconds(0);
    return std::chrono::microseconds(fftTimeAccumulator_us/fftCounter);
}
us PerformanceManager::getAverageRenderTime() const
{
    if (renderCounter == 0) return std::chrono::microseconds(0);
    return std::chrono::microseconds(renderTimeAccumulator_us/renderCounter);
}
us PerformanceManager::getAverageFrameTime() const
{
    if (frameCounter == 0) return std::chrono::microseconds(0);
    return std::chrono::microseconds(frameTimeAccumulator_us/frameCounter);
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

    const timePoint end = std::chrono::steady_clock::now();
    const us duration = calculateTimeDifference(systemStartTime, end);

    systemTimerRunning = false;

    return duration;
}
