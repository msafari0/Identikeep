MACRO(SETSTATIC libName)
if (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(CMAKE_EXE_LINKER_FLAGS " -static")
    target_link_libraries(${libName} -static-libgcc -static-libstdc++)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "PGI")
    message ("Static not implemented.")
else ()
    set(CMAKE_EXE_LINKER_FLAGS " -static")
    target_link_libraries(${libName} -static-libgcc -static-libstdc++)
endif()
ENDMACRO()
