cmake_minimum_required(VERSION 3.10)
project(proj-3-networked-spell-checker-apsawicki)

set(CMAKE_CXX_STANDARD 14)

add_executable(proj-3-networked-spell-checker-apsawicki main.cpp main.h)

SET(CMAKE_CXX_FLAGS -pthread)