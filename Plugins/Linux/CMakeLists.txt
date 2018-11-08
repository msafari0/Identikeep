FOREACH(subdir CPU Mem OS Libs Env Exec FS)
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()
