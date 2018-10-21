cmake_minimum_required (VERSION 3.5)
project (CBlue)

#version number
set (CBlue_VERSION_MAJOR 1)
set (CBlue_VERSION_MINOR 0)
set(CMAKE_CXX_STANDARD 14)

#include the subdirectory containing our libs
#add_subdirectory ()
#include_directories()

#indicate the entry point for the executable
add_executable (CBlue cbluetest.cpp)

# Indicate which libraries to include during the link process.
target_link_libraries (CBlue bluetooth)

install (TARGETS CBlue DESTINATION bin)
