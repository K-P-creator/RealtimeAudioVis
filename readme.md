# Realtime Audio Vis

A real-time audio visualization engine built with OpenGL and WASAPI.

## 3.0 Version Info

Version 3.0 will focus initially on improving diversity of the performance data that is collected. Some metrics I will collect:

1. Frame times
2. FPS
3. Component times (WASAPI call, FFT, Draw sequence)
4. More to come after these are implemented

I will implement a framerate limiter as well as a double buffer system to display only the most current audio samples whenever limiter is enabled.

There will be new fragment shaders that allow for custom color modes for bars. 

A framerate independent smoothing algorithm will be implemented. I plan on calculating a maximum bar velocity based on dt for frames. 

I will also be switching from MSVC to MinGW for more portability in the future.

---

## Project Info

**Stack**: OpenGL, GLFW, imgui, kissFFT, vcpkg, cmake

**Toolchain**: I use MinGW - your experience may vary if you pick another.

I now support three visualization modes - Default, Symmetric, and Double Symmetric. These are all implemented by using different geometry shaders.

### V2.0 Demo

*Note that this demo is at 2x speed to keep the file size down*

https://github.com/user-attachments/assets/9551f810-fc6d-41e6-b236-5088b9b88612

### Build instructions

**Note** - This will only build on Windows, I use the WASAPI to sample system sound.

This branch uses vcpkg for dependencies. The following assumes your vcpkg is in the `C:/` directory.

Ensure that MinGW and CMake are installed and in the path, then configure CMake with:

```cmd
cmake -S . -B build-mingw -G "MinGW Makefiles" ^
-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
-DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic ^
-DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

After, build with:

```cmd
cmake --build build-mingw -j
```

---

## Design

The data flow is as follows:

WASAPI for audio samples -> kissFFT -> openGL pipeline (vertex->geometry->fragment)

### Components (V2.0)

**Audio Manager**

Monolithic class that handles the entire data flow.

### Components (V3.0)

V3 will break up the audio manager into separate classes. Here is the current design (work in progress):

**Audio Sampler**

Abstract class that has implementations for Windows and eventually Linux audio sampling.

**Audio Manager**
 
This class will be majorly refactored. It will handle the FFT transform and any other CPU side adjustments made to audio data (smoothing etc.).

**Render Manager**

Contains all  OpenGL API calls and handles everything related to graphics (shader compilation, uniform binding etc.).

**GUI Manager**

Contains all imgui API calls.

**Performance Manager**

Collects all performance data, and will store to a JSON file when running benchmarks.

---

## Benchmarks

*Benchmarks to come for V3.0*

---

## Tests

The testing suite for AudioVis uses the **Google Test** framework. There is a test executable available to run in the build directory after building.  

No new tests added for V3.0 yet, coming soon.

### CI

The CI runs through **GitHub actions**. Note that there will be no valid audio device in this case. In order to fully validate all tests, tests must be run locally with a valid audio device.

---

## Older Versions

All older versions have their own releases.

The original version used SFML and no OpenGL at all.

V2.0 switched to OpenGL and added a bunch of new stuff including Gtest, CI and more.
