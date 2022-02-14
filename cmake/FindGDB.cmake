find_program(GDB_EXECUTABLE NAMES gdb)

if (GDB_EXECUTABLE)
    execute_process(
        COMMAND ${GDB_EXECUTABLE} --version
        OUTPUT_VARIABLE GDB_VERSION_OUTPUT
    )

    if (NOT GDB_VERSION_OUTPUT MATCHES "GNU gdb \\(GDB\\) ([0-9]+\.[0-9]+)")
        message(FATAL_ERROR "Couldn't get gdb version number from output: ${GDB_VERSION_OUTPUT}")
    endif()

    set(GDB_VERSION_STRING ${CMAKE_MATCH_1})
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GDB
    REQUIRED_VARS GDB_EXECUTABLE
    VERSION_VAR GDB_VERSION_STRING
)

