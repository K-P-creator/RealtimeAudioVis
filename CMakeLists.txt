cmake_minimum_required(VERSION 3.28)
project(AudioVis)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Create kissfft as its own static library
add_library(kissfft STATIC external/kissfft/kiss_fft.c)
target_include_directories(kissfft PUBLIC external/kissfft)

add_executable(AudioVis src/main.cpp src/AudioManager.cpp src/Curve.cpp src/PolyCurve.cpp)

target_compile_features(AudioVis PRIVATE cxx_std_20)

# glad/glfw3/imgui/OpenGL dependencies
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)