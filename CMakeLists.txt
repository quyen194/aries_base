# CMakeList.txt : CMake project for AriesBase, include source and define

cmake_minimum_required (VERSION 3.20)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("aries_base" LANGUAGES C CXX)

# Add main library
add_subdirectory(aries_base)
