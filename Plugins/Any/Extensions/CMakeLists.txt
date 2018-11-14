# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp Extensions.cpp)

add_library(Extensions MODULE ${SOURCES})

