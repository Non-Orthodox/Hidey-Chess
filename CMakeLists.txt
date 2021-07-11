cmake_minimum_required(VERSION 3.5)

# Project Name
project(Hidey-Chess VERSION 0.000001)

# C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

# Setting Module Path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Executable
add_executable(Hidey-Chess main.cpp piece.cpp SDLevents.cpp basicVisuals.cpp board.cpp)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# target_link_libraries(Hidey-Chess SDL2 SDL2_image)
# include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(Hidey-Chess SDL2 ${SDL2_IMAGE_LIBRARIES})