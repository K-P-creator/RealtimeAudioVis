# GLFW requires version 3.16 of CMake or higher
cmake_minimum_required(VERSION 3.16...3.28 FATAL_ERROR)
project("AudioVis" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/main.cpp src/AudioManager.cpp)

# glad/glfw3/imgui/OpenGL/kissfft dependencies
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(kissfft REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kissfft::kissfft-float)