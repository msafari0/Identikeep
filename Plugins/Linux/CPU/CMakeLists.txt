# define minimum cmake version
cmake_minimum_required(VERSION 3.1)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp CPU.cpp)

if (NO_PLUGINS)
    add_library(InfoCPU STATIC ${SOURCES})
else()
    add_library(InfoCPU MODULE ${SOURCES})
endif()

if(RE_PROVIDER STREQUAL "re2")
    target_link_libraries(InfoCPU re2)
endif()
