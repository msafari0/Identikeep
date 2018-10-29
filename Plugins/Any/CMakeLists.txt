

FOREACH(subdir MPI)
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

if (USE_OpenMP)
  ADD_SUBDIRECTORY(OpenMP)
endif()

find_package(MKL)
if(MKL_FOUND)
  ADD_SUBDIRECTORY(MKL)
endif()
