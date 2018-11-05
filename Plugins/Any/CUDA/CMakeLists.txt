# define minimum cmake version
cmake_minimum_required(VERSION 3.1)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp CUDA.cu)

cuda_add_library(InfoCUDA MODULE ${SOURCES})



