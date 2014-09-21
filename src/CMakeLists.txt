cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
SET(CMAKE_CXX_FLAGS "-std=c++0x")

project(ffdropenc)

add_executable(ffdropenc ffdropenc.cxx)