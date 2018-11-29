cmake_minimum_required (VERSION 3.7)

include_directories(../include)

set(SOURCE_FILES client.cpp 
                
                ../include/helper.h 
                )

add_executable(client_test ${SOURCE_FILES})

target_link_libraries (client_test pthread)