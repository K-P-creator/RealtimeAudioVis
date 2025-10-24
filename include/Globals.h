#pragma once

// Used for SMFL window
constexpr unsigned int WINDOW_HEIGHT = 1080;
constexpr unsigned int WINDOW_WIDTH = 1920;

// Used for frame timing - lower values = more often = more CPU usage
// Use 17 for ~ 60fps
#define FRAME_TIME chrono::milliseconds(17)

// Number of frequency range bars to draw
// Not being used yet
constexpr unsigned int BAR_COUNT = 64;

// How many times to retry initializing am if error occurs
constexpr unsigned int RETRY_COUNT = 5;

// How many frames are needed for kiss fft to gen a audio sample
constexpr int FFT_COUNT = 480;

#define REFTIMES_PER_SEC 1000000;

#define SMOOTHING_COEF 0.99999f // Must be between zero and one