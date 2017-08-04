set (CMAKE_C_STANDARD 11)

cmake_minimum_required(VERSION 3.1)

project(compressor)

set(SOURCE_EXE main.c)

set(SOURCE_LIB utils.c ari.c ppm.c bwt.c)

add_library(utils STATIC ${SOURCE_LIB})

add_executable(compress ${SOURCE_EXE})

target_link_libraries(compress utils)