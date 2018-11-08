cmake_minimum_required (VERSION 3.5)
project (blueAgent)

#version number
set (CBlue_VERSION_MAJOR 1)
set (CBlue_VERSION_MINOR 0)
set (CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/cbluemain.cpp src/helper.cpp src/BlueServer.cpp src/agent.cpp include/BlueServer.h include/helper.h include/agent.h)

#indicate the entry point for the executable
add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

#include the subdirectory containing our headers
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Indicate which libraries to include during the link process.
target_link_libraries (${PROJECT_NAME} 
                bluetooth 
                ssl 
                crypto
                )

install (TARGETS blueAgent DESTINATION bin)