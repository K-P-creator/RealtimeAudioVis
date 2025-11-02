# Realtime Audio Vis

## Info

Visable audio frequencies range from ~ 1Hz = 7kHz

NOTE this will change in future versions. I'm thinking 1-20kHz maybe with logarithmic x scaling...

### Versions

There is a version using SFML. That version currently has more features with per bar coloring and double symmetric mode. 

The new version does not have any frame limiter in place, so the smoothing will be much less noticable that the SFML version.

### Design

I try to do as much stuff possible on the GPU side here. The basic flow of the main visualization pipeline is as follows:

Capture Audio with CPU and format it (smoothing on/off)

### Branches

There is currently an openGL branch where I am working on migrating to openGL rendering rather than using SFML. It is still a work in progress.

## Usage

Vertex shader places vertices 1-1 with points and fills in z/w coordinates

Geometric shader places the other 3 vertices per bar, this shader varies based off display mode and takes a uniform bar count for calculations

Fragment shader takes a uniform color and draws bars

I will eventually use GPU for smoothing calculations. This would be done in the vertex shader.

I will also add per bar colors. This will be done in the fragment shader.

### Build instructions

This branch uses vcpkg for dependencies

build with `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_POLICY_VERSION_MINIMUM=3.5`

substituting your local vcpks path and possibly your target triplet.

Then open the .sln in build and f5.

