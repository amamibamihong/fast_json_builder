cmake_minimum_required(VERSION 3.21)
project(simple_json_builder C)

set(CMAKE_C_STANDARD 99)

add_executable(test json_util.c
        json_util.h
        main.c
        strbuffer.c
        strbuffer.h)
