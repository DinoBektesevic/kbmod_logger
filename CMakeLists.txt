cmake_minimum_required(VERSION 3.4...3.18)
project(logger)

add_subdirectory(pybind11)
pybind11_add_module(logger src/logger_bindings.cpp)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(logger
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

target_compile_options(logger PRIVATE $<$<COMPILE_LANGUAGE:CXX>:
    -g
    -Og
>)
