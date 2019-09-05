cmake_minimum_required (VERSION 3.10)

project (blueAgent)

#version number
set (CBlue_VERSION_MAJOR 1)
set (CBlue_VERSION_MINOR 0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CLANG_TARGET_TRIPLE armv7a-none-eabi)
set(CMAKE_CXX_COMPILER clang++)


set(SOURCE_FILES src/cbluemain.cpp 
                src/helper.cpp              
                src/hciManager.cpp         
                include/helper.h 
                include/hciManager.h
                )

#indicate the entry point for the executable
add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

#include the subdirectory containing our headers
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Indicate which libraries to include during the link process.
target_link_libraries (${PROJECT_NAME} 
                stdc++fs
                pthread
                bluetooth 
                ssl 
                crypto
                curl
                )

add_subdirectory(test)

# install exe to bin
# install (TARGETS blueAgent DESTINATION bin)
