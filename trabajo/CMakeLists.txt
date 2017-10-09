cmake_minimum_required(VERSION 3.7)
project(trabajo)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Matriz.cpp Matriz.h)
add_executable(trabajo ${SOURCE_FILES})