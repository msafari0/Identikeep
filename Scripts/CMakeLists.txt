# define minimum cmake version
cmake_minimum_required(VERSION 3.1)


find_package( PythonInterp 2.7 )

if(PYTHONINTERP_FOUND)
    configure_file(identiview.py ${EXECUTABLE_OUTPUT_PATH}/identiview @ONLY)
endif()


find_program(MPI_RUN mpirun)
find_program(BASH bash)

if(NOT MPI_RUN)
    message("mpirun not found. Installation of identirun skipped")
else()
    configure_file(identirun.py ${EXECUTABLE_OUTPUT_PATH}/identirun @ONLY)
endif()
