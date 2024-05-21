cmake_minimum_required(VERSION 3.5)

# Project Name
project(Hidey-Chess VERSION 0.00001)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")

# Setting Module Path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIRECTORY}")

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# Core Library
set(CORE core/log.cpp
  core/duck-lisp.cpp
  core/scripting.cpp
  core/render_window.cpp
  core/repl.cpp
  core/gui.cpp
  core/game_model/board.cpp
  core/file_utilities.cpp)

add_definitions(-DUSE_PARENTHESIS_INFERENCE)

add_library(HideyCore ${CORE})
target_include_directories(HideyCore
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

# Executables
add_executable(Hidey-Chess
  main.cpp
  )

add_subdirectory(duck-lisp build-lisp)
add_subdirectory(tests)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

target_link_libraries(Hidey-Chess PUBLIC SDL2 ${SDL2_IMAGE_LIBRARIES} HideyCore DuckLisp)
