//  Performance manager will serve to monitor the performance of all system components.
//  This will be used when running benchmarks.

#include <chrono>
#include <vector>

class PerformanceManager {

    using us = std::chrono::microseconds;   //  Measure with integer microseconds (us)
    using timePoint = std::chrono::steady_clock::time_point;

public:
    PerformanceManager();
    ~PerformanceManager();
    PerformanceManager(const PerformanceManager &) = delete;
    PerformanceManager &operator=(const PerformanceManager &) = delete;


    void startFrameTimer();
    us stopFrameTimer();    //  Returns frame time in us

    void startFFTTimer();
    us stopFFTTimer();      //  Returns FFT time in us

    void startRenderTimer();
    us stopRenderTimer();     //  Returns render time in us

    void writePerformanceData();    //  Writes a performance report to json in benchmarks/out
                                    //  Report metrics - average/min/max/p50/p95/p99 for each timer

private:
    void startSystemTimer();    // Called in constructor
    us stopSystemTimer();     // Called inside writePerformanceData()

    timePoint systemStartTime;
    timePoint startFrameTime;
    timePoint startFFTTime;
    timePoint startRenderTime;

    us systemRunTime = std::chrono::microseconds(0);

    //  Timer flags
    bool systemTimerRunning = false;
    bool frameTimerRunning = false;
    bool fftTimerRunning = false;
    bool renderTimerRunning = false;

    unsigned long long frameCounter = 0;
    unsigned long long fftCounter = 0;
    unsigned long long renderCounter = 0;

    //  I have yet to choose a parsing strategy. Probably binning the data in a histogram as it is parsed.
    //  void binFrameTime(us);
    //  void binFFTTime(us);
    //  void binRenderTime(us);
};