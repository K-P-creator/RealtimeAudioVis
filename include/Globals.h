#pragma once

// Used for SMFL window
constexpr unsigned int WINDOW_HEIGHT = 1080;
constexpr unsigned int WINDOW_WIDTH = 1920;


// Number of frequency range bars to draw
// Not being used yet
constexpr unsigned int BAR_COUNT = 64;

// How many times to retry initializing am if error occurs
constexpr unsigned int RETRY_COUNT = 5;

// How many frames are needed for kiss fft to gen a audio sample
constexpr int FFT_COUNT = 480;

#define REFTIMES_PER_SEC 1000000;

#define SMOOTHING_COEF 0.9f // Must be between zero and one

// bool smoothing, uint displayModeIndex, float[3] baseColorBars, float barHeightScaling, int windowHeight, int windowWidth
#define DEFAULT_SETTINGS false, 0, {1.0f, 0.0f, 0.0f}, 0.8f, 1080, 1920