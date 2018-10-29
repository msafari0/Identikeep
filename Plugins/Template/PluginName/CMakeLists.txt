# define minimum cmake version
cmake_minimum_required(VERSION 3.1)

SETCPP11()

include_directories("../../../include")

set(SOURCES plugin.cpp PluginName.cpp)

if (NO_PLUGINS)
    add_library(InfoPluginName STATIC ${SOURCES})
else()
    add_library(InfoPluginName MODULE ${SOURCES})
endif()

# possibly find required packages
#find_package(SOMETHING REQUIRED)
#if(SOMETHING_FOUND)
#    include_directories(${SOMETHING_INCLUDE_DIRS})
#    link_libraries(InfoMKL ${SOMETHING_LIBRARIES}) 
#endif()


