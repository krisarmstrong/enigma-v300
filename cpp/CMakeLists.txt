cmake_minimum_required(VERSION 3.28)
project(enigma_v300_pure_cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(enigma_v300_pure_cpp
        src/enigma_v300_pure_cpp.cpp)
