cmake_minimum_required(VERSION 3.14)
project(gl_validation_layer C CXX)

set(CMAKE_CXX_STANDARD 17)

option(GL_VALIDATION_LAYER_BUILD_TESTS "Build tests for the OpenGL Validation Layer" OFF)

add_library(gl_validation_layer src/context.cpp)

target_include_directories(gl_validation_layer PUBLIC include/)

# TODO: Check compiler and only set flags when available
target_compile_options(gl_validation_layer PRIVATE -Wall -Werror
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-newline-eof
        -Wno-unused-parameter
)

if (${GL_VALIDATION_LAYER_BUILD_TESTS})
    message(STATUS "OpenGL Validation Layer - Testing enabled. Set GL_VALIDATION_LAYER_BUILD_TESTS to OFF to disable.")
    add_subdirectory(tests)
endif()