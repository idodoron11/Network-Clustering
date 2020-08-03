cmake_minimum_required(VERSION 3.15)
project(cluster C)

set(CMAKE_C_STANDARD 90)
set(GCC_COVERAGE_COMPILE_FLAGS "-ansi -Wall -Wextra -Werror -pedantic-errors")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(cluster cluster.c spmat.c matrix.h matrix.c graph.h graph.c VerticesGroup.h VerticesGroup.c)