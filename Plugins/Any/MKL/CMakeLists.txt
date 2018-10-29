# define minimum cmake version
cmake_minimum_required(VERSION 3.1)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp MKL.cpp)

if (NO_PLUGINS)
    add_library(InfoMKL STATIC ${SOURCES})
else()
    add_library(InfoMKL MODULE ${SOURCES})
endif()

find_package(MKL REQUIRED)
if(MKL_FOUND)
    include_directories(${MKL_INCLUDE_DIRS})
    link_libraries(InfoMKL ${MKL_LIBRARIES}) 
endif()


