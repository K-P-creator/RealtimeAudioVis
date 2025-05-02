cmake_minimum_required(VERSION 3.28)
project(AudioVis LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set optimization flags for release mode
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")

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
set_target_properties(kissfft PROPERTIES LINKER_LANGUAGE C)


add_executable(AudioVis "src/main.cpp" "src/AudioManager.cpp")
target_compile_features(AudioVis PRIVATE cxx_std_20)

target_include_directories(AudioVis PRIVATE external/kissfft)

target_link_libraries(AudioVis PRIVATE SFML::Graphics
        SFML::Window SFML::System SFML::Audio kissfft)