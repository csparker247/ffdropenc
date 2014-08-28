cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(ffdropenc)

set(Boost_USE_STATIC_LIBS     ON)
set(Boost_USE_STATIC_RUNTIME   ON)
find_package(Boost 1.55.0 REQUIRED COMPONENTS program_options)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(ffdropenc ffdropenc.cxx)

target_link_libraries(ffdropenc ${Boost_LIBRARIES})