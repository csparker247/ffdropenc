cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project("ffdropenc" VERSION 2.0.0)

include(FetchContent)
include(CMakeDependentOption)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Use modern C++ for everything and generate compile_commands database.
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Look for external dependencies.
include(FindDependencies)

# Some helpful constants to be used in subprojects.
include(Constants)

# Hack to get ninja output colorized for all source files.
if (CMAKE_GENERATOR MATCHES "Ninja")
    add_compile_options(-fdiagnostics-color=always)
endif()

# Documentation
find_package(Doxygen OPTIONAL_COMPONENTS dot)
CMAKE_DEPENDENT_OPTION(FFDE_BUILD_DOCS "Build Doxygen documentation" on "DOXYGEN_FOUND" off)
CMAKE_DEPENDENT_OPTION(FFDE_INSTALL_DOCS "Install documentation" off "FFDE_BUILD_DOCS" off)
if(FFDE_BUILD_DOCS)
    add_subdirectory(docs)
endif()

## Libraries ##
add_subdirectory(libdropenc)

## Core Apps ##
add_subdirectory(apps)

# Install to system directories
include(Install)
