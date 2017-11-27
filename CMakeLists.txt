cmake_minimum_required(VERSION 3.8)
project(mm)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c include/logger.h logger.c include/mmemory.h mmemory.c include/page_segment.h page_segment.c include/cache.h cache.c include/assertion.h include/hard_drive.h hard_drive.c include/ram.h ram.c include/err_codes.h tests/test_init.h tests/test_malloc.h tests/test_free.h tests/test_read.h tests/test_write.h)
add_executable(mm ${SOURCE_FILES})