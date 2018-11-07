
FOREACH(subdir MPI)
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

FOREACH(subdir MPIBench)
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

FOREACH(subdir IOBench)
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

if (USE_OpenMP)
  ADD_SUBDIRECTORY(OpenMP)
endif()

find_package(MKL)
if(MKL_FOUND)
  ADD_SUBDIRECTORY(MKL)
endif()

find_package(CUDA)
if(CUDA_FOUND)
  ADD_SUBDIRECTORY(CUDA)
endif()
