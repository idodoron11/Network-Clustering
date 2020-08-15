cmake_minimum_required(VERSION 3.15)
project(cluster C)

set(CMAKE_C_STANDARD 90)
set(GCC_COVERAGE_COMPILE_FLAGS "-ansi -Wall -Wextra -Werror -pedantic-errors")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(cluster cluster.c spmat.c matrix.h matrix.c graph.h graph.c LinkedList.h LinkedList.c VerticesGroup.h VerticesGroup.c division.h division.c defs.h defs.c ErrorHandler.c ErrorHandler.h)
add_executable(tester tests/tester.h tests/tester.c spmat.c matrix.h matrix.c graph.h graph.c LinkedList.h LinkedList.c VerticesGroup.h VerticesGroup.c division.h division.c defs.h defs.c ErrorHandler.c ErrorHandler.h tests/testUtils.c tests/testUtils.h)
add_executable(neoTester tests/neoTests/main.c cluster.c spmat.c matrix.h matrix.c graph.h graph.c LinkedList.h LinkedList.c VerticesGroup.h VerticesGroup.c division.h division.c defs.h defs.c ErrorHandler.c ErrorHandler.h tests/cluster.h tests/testUtils.c tests/testUtils.h)