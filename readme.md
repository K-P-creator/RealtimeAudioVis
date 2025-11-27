# Realtime Audio Vis

## Info

Stack: openGL, GLFW, ImGui, kissFFT, vcpkg, cmake

Visable audio frequencies range from ~ 1Hz - 7kHz

NOTE this will change in future versions. I'm thinking 1-20kHz maybe with logarithmic x scaling...

I now support three visualization modes - Default, Symmetric, and Double Symmetric. These are all implemented by using different geometry shaders.

### Versions

There is a version using SFML. That version currently has more features with per bar coloring being the last feature I have yet to implement in openGL. 

The new version does not have any frame limiter in place, so the smoothing will be much less noticable that the SFML version.

A framerate independent smoothing method, and GPU based smoothing calculations are on the to do list here.

### Design

The pipline flow is as follows:

Get audio sample, smooth it (optional) and translate to 2D vertices CPU side.

Vertex shader places vertices 1-1 with points and fills in z/w coordinates.

Geometric shader places the other 3 vertices per bar, this shader varies based off display mode and takes a uniform bar count for calculations. Every mode will perform some sort of transformation on the input vertices, turning them into triangle strips with 4 vertices. The double symmetric will duplicate them into two 4 vertex triangle strips. 

Fragment shader takes a uniform color and draws bars.

I will eventually use GPU for smoothing calculations. This would be done in the vertex shader.

I will also add per bar colors. This will be done in the fragment shader.

### Build instructions

This branch uses vcpkg for dependencies

build with 

```cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_POLICY_VERSION_MINIMUM=3.5```

substituting your local vcpks path and possibly your target triplet.

Set the startup project to be AudioVis

Then open the .sln and build/run with f5 (or the green arrow).


### Demo

*Note that this demo is at 2x speed to keep the file size down*

https://github.com/user-attachments/assets/9551f810-fc6d-41e6-b236-5088b9b88612






