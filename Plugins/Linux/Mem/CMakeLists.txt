# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp Mem.cpp)

if (NO_PLUGINS)
    add_library(InfoMem STATIC ${SOURCES})
else()
    add_library(InfoMem MODULE ${SOURCES})
endif()

if(RE_PROVIDER STREQUAL "re2")
    target_link_libraries(InfoMem re2)
endif()
