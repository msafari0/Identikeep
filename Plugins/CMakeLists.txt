
ADD_SUBDIRECTORY(Any)

# Platform specific plugins
#   see https://itk.org/Wiki/CMake/Examples#Check_operating_system
#       
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  ADD_SUBDIRECTORY(Mac)
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  ADD_SUBDIRECTORY(Win)
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  ADD_SUBDIRECTORY(Linux)
endif()
