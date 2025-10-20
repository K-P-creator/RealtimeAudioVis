# Realtime Audio Vis

## Info

### OpenGL branch

This branch will switch to using openGL, glfw and imgui rather than SFML

### Build instructions

This branch uses vcpkg for dependencies

build with `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows`

substituting your local vcpks path and possibly your target triplet.

Then open the .sln in build and f5.
