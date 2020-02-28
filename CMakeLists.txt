cmake_minimum_required(VERSION 3.0)
project(KillerSudoku)
set(SOURCES 
    KillerSudoku.cpp
    NumberSet.cpp
    Sum.cpp
)

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(CodeRoyale ${SOURCES})