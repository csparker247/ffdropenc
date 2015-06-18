project(ffdropenc)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Use custom package finders
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(LibConfig++ REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS system filesystem program_options)

include_directories(${LIBCONFIG++_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

set(DROP_REQ_LIBS ${LIBCONFIG_LIBRARY} ${Boost_LIBRARIES})

add_executable(ffdropenc src/ffdropenc.cpp src/Video.cpp)
target_link_libraries(ffdropenc ${DROP_REQ_LIBS})