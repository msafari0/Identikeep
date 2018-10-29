# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp OMP.cpp)

if (NO_PLUGINS)
    add_library(InfoOMP STATIC ${SOURCES})
else()
    add_library(InfoOMP MODULE ${SOURCES})
endif()


IF(USE_OpenMP)
  FIND_PACKAGE(OpenMP)
  IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  ENDIF()
ENDIF()
