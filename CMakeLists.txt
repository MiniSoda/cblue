cmake_minimum_required (VERSION 3.5)
project (blueAgent)

#version number
set (CBlue_VERSION_MAJOR 1)
set (CBlue_VERSION_MINOR 0)
set (CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/cbluemain.cpp)

#include the subdirectory containing our libs
include_directories(src)
link_directories(src)
add_subdirectory(src)

#indicate the entry point for the executable
add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

# Indicate which libraries to include during the link process.
target_link_libraries (${PROJECT_NAME}
                source 
                bluetooth 
                ssl 
                crypto
                )

install (TARGETS blueAgent DESTINATION bin)
