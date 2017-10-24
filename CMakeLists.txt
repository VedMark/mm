cmake_minimum_required(VERSION 3.8)
project(memomy_manager)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c mmemory.h mmemory.c page.c page.h segment.h segment.c)
add_executable(memomy_manager ${SOURCE_FILES})