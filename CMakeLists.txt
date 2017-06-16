cmake_minimum_required(VERSION 3.7)
project(ts1_1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" )

set(SOURCE_FILES main.c)
add_executable(ts1_1 ${SOURCE_FILES})