# define minimum cmake version
cmake_minimum_required(VERSION 3.1)

SETCPP11()


set(SOURCES identikeep.cpp PCollect.cpp Files.cpp MPIntercept.cpp PluginLoader.cpp)
add_executable(identikeep ${SOURCES})

set(SOURCES PCollect.cpp Files.cpp MPIntercept.cpp PluginLoader.cpp)
add_library(MPIntercept MODULE ${SOURCES})

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})
include_directories("../include")


if(MPI_COMPILE_FLAGS)
  set_target_properties(identikeep PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(identikeep PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

target_link_libraries(identikeep 
    ${CMAKE_DL_LIBS}
    ${MPI_LIBRARIES}
)

if (Boost_FOUND)
  target_link_libraries( PCollect ${Boost_LIBRARIES} )
endif()

IF(USE_OpenMP)
  FIND_PACKAGE(OpenMP)
  IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  ENDIF()
ENDIF()
