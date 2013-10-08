project(gunpowder)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x -O2 -g -lm")

#header files compilation
add_custom_target(Headers SOURCES Timer.hpp definitions.hpp Engine.hpp Board.hpp)

add_executable(${PROJECT_NAME} main.cpp)

