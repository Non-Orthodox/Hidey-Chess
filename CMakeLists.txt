cmake_minimum_required(VERSION 3.5)

# Project Name
project(Hidey-Chess VERSION 0.000001)

# Executable
add_executable(Hidey-Chess main.cpp)

find_package(SDL2)

target_link_libraries(Hidey-Chess SDL2)


