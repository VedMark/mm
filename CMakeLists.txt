cmake_minimum_required(VERSION 3.8)
project(mm)

set(CMAKE_C_STANDARD 11)
set(SOURCE_FILES main.c include/logger.h logger.c include/mmemory.h mmemory.c include/page_segment.h page_segment.c include/cache.h cache.c include/assertion.h include/hard_drive.h hard_drive.c include/ram.h ram.c include/err_codes.h tests/include/tests.h tests/load_test.c tests/test_free.c tests/test_init.c tests/test_malloc.c tests/test_read.c tests/test_write.c)
add_executable(mm ${SOURCE_FILES})