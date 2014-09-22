project(ffdropenc)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_OSX_ARCHITECTURES x86_64)

# libconfig++
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(LibConfig++ REQUIRED)
include_directories(${LIBCONFIG++_INCLUDE_DIR})
set(CMAKE_EXE_LINKER_FLAGS "-lconfig++")

add_executable(ffdropenc ffdropenc.cxx)

target_link_libraries(ffdropenc ${LIBCONFIG_LIBRARY})