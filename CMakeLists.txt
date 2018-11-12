# define minimum cmake version required for c++11 support,
# can be lowered to 2.8 if c++11 flags are passed by hand but

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

project(Squidly)

include(FindMKL)
include(SubdirList)
include(SetCPP11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${DESC})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${DESC})

# Options
option (USE_OpenMP "Use OpenMP" ON)



add_subdirectory(Plugins)
add_subdirectory(PCollect)
add_subdirectory(Scripts)

