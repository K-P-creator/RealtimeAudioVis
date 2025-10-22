# Realtime Audio Vis

## Info

### OpenGL branch

This branch will switch to using openGL, glfw and imgui rather than SFML.

I also have switched to using vcpkg for all deps, including kiss_fft.

### Build instructions

This branch uses vcpkg for dependencies

build with `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_POLICY_VERSION_MINIMUM=3.5`

substituting your local vcpks path and possibly your target triplet.

Then open the .sln in build and f5.
