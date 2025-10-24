#pragma once

// Number of frequency range bars to draw
// Not being used yet
constexpr unsigned int BAR_COUNT = 64;

// How many times to retry initializing am if error occurs
constexpr unsigned int RETRY_COUNT = 5;

// How many frames are needed for kiss fft to gen a audio sample
constexpr int FFT_COUNT = 480;

#define REFTIMES_PER_SEC 1000000;

// bool smoothing, uint displayModeIndex, float[3] baseColorBars, float barHeightScaling, int windowHeight, int windowWidth, float smoothingCoef,
#define DEFAULT_SETTINGS true, 0, {1.0f, 0.0f, 0.0f}, 1.0f, 1080, 1920, 0.9f