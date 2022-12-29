add_executable(btest btesting.cpp)
target_link_libraries(btest PUBLIC SDL2 ${SDL2_IMAGE_LIBRARIES} HideyCore DuckLisp)

add_executable(jtest jtesting.cpp)
target_link_libraries(jtest PUBLIC SDL2 ${SDL2_IMAGE_LIBRARIES} HideyCore DuckLisp)
