# define minimum cmake version
cmake_minimum_required(VERSION 3.0.2)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp Libs.cpp)

if (NO_PLUGINS)
    add_library(InfoLibs STATIC ${SOURCES})
else()
    add_library(InfoLibs MODULE ${SOURCES})
endif()

