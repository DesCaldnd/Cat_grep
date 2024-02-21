cmake_minimum_required(VERSION 3.22)
project(Lab1 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra")

add_executable(Cat cat/main.c)
add_executable(Grep grep/main.c grep/structs.h grep/structs.c)