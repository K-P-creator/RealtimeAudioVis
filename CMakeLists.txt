cmake_minimum_required(VERSION 3.28)
project(AudioVis)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Fetch SFML from github
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Create kissfft as its own static library
add_library(kissfft STATIC external/kissfft/kiss_fft.c)
target_include_directories(kissfft PUBLIC external/kissfft)

add_executable(AudioVis src/main.cpp src/AudioManager.cpp src/Curve.cpp src/PolyCurve.cpp)
target_compile_features(AudioVis PRIVATE cxx_std_20)
target_link_libraries(AudioVis PRIVATE SFML::Graphics SFML::Audio kissfft)