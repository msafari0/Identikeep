# define minimum cmake version
cmake_minimum_required(VERSION 3.0.2)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp OS.cpp)

if (NO_PLUGINS)
    add_library(InfoOS STATIC ${SOURCES})
else()
    add_library(InfoOS MODULE ${SOURCES})
endif()

