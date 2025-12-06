# Realtime Audio Vis

## Info

Stack: OpenGL, GLFW, ImGui, kissFFT, vcpkg, cmake

Visable audio frequencies range from ~ 1Hz - 7kHz

NOTE this will change in future versions. I'm thinking 1-20kHz maybe with logarithmic x scaling...

I now support three visualization modes - Default, Symmetric, and Double Symmetric. These are all implemented by using different geometry shaders.

### Versions

There is a version using SFML. That version currently has more features with per bar coloring being the last feature I have yet to implement in OpenGL.

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

### Benchmarks

All benchmarks were run in release mode.

Here are the benchmarks from the deprecated **SFML** version:

```ps
Average per frame render time for 10,000 frames with mode default:                      0.000128855s
Switching modes...
Average per frame render time for 10,000 frames with mode symmetric:                    0.000184604s
Switching modes...
Average per frame render time for 10,000 frames with mode double symmetric:             0.000187429s
Switching modes...
Average per frame render time for 10,000 frames with mode double sym. with smoothing:   0.000183666s
Switching modes...
Average per frame render time for 10,000 frames with mode curved:                       0.0078988s
```

Note that there is a "curved" mode here, which I do not plan on implementing in the new OpenGL version.

Here are the benchmarks for the **OpenGL** version (current):

```ps
Average per frame render time for 10,000 frames with mode default:                      6.81027e-06s (0.00000681s)
Switching modes...
Average per frame render time for 10,000 frames with mode symmetric:                    8.08056e-06s (0.00000808s)
Switching modes...
Average per frame render time for 10,000 frames with mode double symmetric:             1.00012e-05s (0.00001000s)
Switching modes...
Average per frame render time for 10,000 frames with mode double sym. with smoothing:   1.01402e-05s (0.00001014s)
```

Here are the speedups between versions for each mode:

```ps
Default Mode:       18.9x speedup
Symmetric Mode:     22.8x speedup
Double Sym Mode:    18.7x speedup
DblSym Smth Mode:   18.1x speedup

Average Speedup:    19.6x speedup
```

### Build instructions

This branch uses vcpkg for dependencies

build with

```cmd
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

substituting your local vcpks path and possibly your target triplet.

Set the startup project to be AudioVis

Then open the .sln and build/run with f5 (or the green arrow).

### Demo

*Note that this demo is at 2x speed to keep the file size down*

https://github.com/user-attachments/assets/9551f810-fc6d-41e6-b236-5088b9b88612







