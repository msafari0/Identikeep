# define minimum cmake version
cmake_minimum_required(VERSION 3.1)


find_package( PythonInterp 2.7 )

if(PYTHONINTERP_FOUND)
    configure_file(identiview.py ${EXECUTABLE_OUTPUT_PATH}/identiview @ONLY)
endif()
