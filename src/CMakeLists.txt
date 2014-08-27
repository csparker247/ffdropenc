cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(ffdropenc)

find_package(Boost 1.55 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(ffdropenc ffdropenc.cxx)

target_link_libraries(ffdropenc ${Boost_LIBRARIES})