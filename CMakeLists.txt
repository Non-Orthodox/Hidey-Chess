cmake_minimum_required(VERSION 3.5)

# Project Name
project(Hidey-Chess VERSION 0.000001)

# C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

# Executable
add_executable(Hidey-Chess main.cpp piece.cpp SDLevents.cpp basicVisuals.cpp)

find_package(SDL2)

target_link_libraries(Hidey-Chess SDL2)


