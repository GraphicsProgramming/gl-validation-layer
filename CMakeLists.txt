cmake_minimum_required(VERSION 3.22)
project(gl_validation_layer)

set(CMAKE_CXX_STANDARD 17)

add_library(gl_validation_layer src/context.cpp)

target_include_directories(gl_validation_layer PUBLIC include/)
target_compile_options(gl_validation_layer PRIVATE -Wall -Werror)