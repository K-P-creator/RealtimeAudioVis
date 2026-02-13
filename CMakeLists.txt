# GLFW requires version 3.16 of CMake or higher
cmake_minimum_required(VERSION 3.16...3.28 FATAL_ERROR)
project("AudioVis" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_executable(${PROJECT_NAME} src/main.cpp src/AudioManager.cpp src/PerformanceManager.cpp)
add_executable("Benchmarks" src/bench.cpp src/AudioManager.cpp src/PerformanceManager.cpp)

file (GLOB TEST_SOURCES tests/test_*.cpp)
add_executable("Tests" ${TEST_SOURCES} src/AudioManager.cpp)

# glad/glfw3/imgui/OpenGL/kissfft dependencies
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries("Benchmarks" PRIVATE glad::glad)
target_link_libraries("Tests" PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries("Benchmarks" PRIVATE glfw)
target_link_libraries("Tests" PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
target_link_libraries("Benchmarks" PRIVATE imgui::imgui)
target_link_libraries("Tests" PRIVATE imgui::imgui)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
target_link_libraries("Benchmarks" PRIVATE OpenGL::GL)
target_link_libraries("Tests" PRIVATE OpenGL::GL)

find_package(kissfft REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kissfft::kissfft-float)
target_link_libraries("Benchmarks" PRIVATE kissfft::kissfft-float)
target_link_libraries("Tests" PRIVATE kissfft::kissfft-float)

find_package(gtest REQUIRED)
target_link_libraries("Tests" PRIVATE GTest::gtest GTest::gtest_main)

if (BUILD_TESTING)
    add_test(NAME AudioVisTests COMMAND Tests)
endif()