FOREACH(subdir CPU Mem OS Libs Env Exec)
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()
