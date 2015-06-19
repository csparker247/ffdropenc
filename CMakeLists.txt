project(ffdropenc)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Use custom package finders
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Boost 1.58.0 REQUIRED COMPONENTS system filesystem program_options)

include_directories(${Boost_INCLUDE_DIRS})

set(DROP_REQ_LIBS ${Boost_LIBRARIES})

add_executable(ffdropenc src/ffdropenc.cpp src/ffcommon.cpp src/Video.cpp src/Preset.cpp src/ffio.cpp)
target_link_libraries(ffdropenc ${DROP_REQ_LIBS})