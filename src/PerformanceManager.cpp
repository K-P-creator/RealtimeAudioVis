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
    if (systemTimerRunning) {
        stopSystemTimer();
        writePerformanceData();
    }
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

    std::cout << std::endl
              << std::endl;
    std::cout << "----------------------------------------------------\n";
    std::cout << "             Performance Summary\n";
    std::cout << "----------------------------------------------------\n";
    std::cout << "System Runtime:      " << getSystemRunTime() << "\n";
    std::cout << "Frames Measured:     " << frameCounter  << "\n";
    std::cout << "FFTs Measured:       " << fftCounter    << "\n";
    std::cout << "Renders Measured:    " << renderCounter << "\n";
    std::cout << std::endl;
    std::cout << "----------------------------------------------------\n";
    std::cout << "Average Frame Time:  " << getAverageFrameTime() << "\n";
    std::cout << "Average FPS:         " << static_cast<unsigned int>(1.0f / (static_cast<double>(getAverageFrameTime().count()) / 10e6)) << "\n";
    std::cout << "Average FFT Time:    " << getAverageFFTTime() << "\n";
    std::cout << "Average Render Time: " << getAverageRenderTime() << "\n";
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

std::chrono::seconds PerformanceManager::getSystemRunTime() const
{
    return std::chrono::seconds(static_cast<int>(static_cast<double>(systemRunTime.count()) / 10e6)); // Divide by 10e6 to get seconds
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
    systemRunTime = duration;

    return duration;
}
