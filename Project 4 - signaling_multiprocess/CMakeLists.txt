cmake_minimum_required(VERSION 3.10)
project(project_4_f21_apsawicki)

set(CMAKE_CXX_STANDARD 14)

add_executable(project_4_f21_apsawicki
        main.cpp)

SET(CMAKE_CXX_FLAGS -pthread)