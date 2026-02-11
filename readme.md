# Realtime Audio Vis

## 3.0 Version Info

Version 3.0 will focus initially on improving diversity of the performance data that is collected. Some metrics I will collect:
1. Frame times
2. FPS
3. Component times (WASAPI call, FFT, Draw sequence)
4. More to come after these are implemented

I will implement a framerate limiter as well as a double buffer system to display only the most current audio samples whenever limiter is enabled.

There will be new fragment shaders that allow for custom color modes for bars. 

A framerate independent smoothing algorithm will be implemented. I plan on calculating a maximum bar velocity based on dt for frames. 

I will also be switching from MSVC to MinGW.

## Info

Stack: OpenGL, GLFW, imgui, kissFFT, vcpkg, cmake

Toolchain: I use MinGW - your experience may vary if you pick another. 

Visible audio frequencies range from ~ 1Hz - 7kHz

NOTE this will change in future versions. I'm thinking 1-20kHz maybe with logarithmic x scaling...

I now support three visualization modes - Default, Symmetric, and Double Symmetric. These are all implemented by using different geometry shaders.

### Older Versions

The original version used SFML and no OpenGL at all. See releases.

V2.0 switched to OpenGL and added a bunch of new stuff including Gtest, CI and more.

### Design

The pipeline flow is as follows:

Get audio sample, smooth it (optional) and translate to 2D vertices CPU side.

Vertex shader places vertices 1-1 with points and fills in z/w coordinates.

Geometric shader places the other 3 vertices per bar, this shader varies based off display mode and takes a uniform bar count for calculations. Every mode will perform some sort of transformation on the input vertices, turning them into triangle strips with 4 vertices. The double symmetric will duplicate them into two 4 vertex triangle strips.

Fragment shader takes a uniform color and draws bars.

I will eventually use GPU for smoothing calculations. This would be done in the vertex shader.

I will also add per bar colors. This will be done in the fragment shader.

### Benchmarks

Benchmarks to come for V3.0

### Build instructions

**Note** - This will only build on Windows (for now)

This branch uses vcpkg for dependencies. The following assumes your vcpkg is at the system root.

Ensure that MinGW and CMake are installed and in the path, then use: 

```cmd
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```
After, build with:

```cmd
cmake --build build-mingw -j
```

### V2.0 Demo

*Note that this demo is at 2x speed to keep the file size down*

https://github.com/user-attachments/assets/9551f810-fc6d-41e6-b236-5088b9b88612

### Tests

The testing suite for AudioVis uses the Google Test framework. There is a test executable available to run in the build directory after building.  

No new tests added for V3.0 yet, coming soon.

### CI

The CI runs through GitHub actions. Note that there will be no valid audio device in this case. In order to fully validate all tests, tests must be run locally with a valid audio device.



