cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
SET(CMAKE_CXX_FLAGS "-std=c++0x")

project(ffdropenc)

# Cereal includes
include_directories(cereal/include)

add_executable(ffdropenc ffdropenc.cxx)