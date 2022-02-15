find_package(Tclsh)

set(TCL_SQLITE3_FOUND FALSE CACHE BOOL "Assume TclSQLite3 is found")

if (TCLSH_FOUND)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo
            "package require sqlite3; sqlite3 db :memory:; puts [db version]"
        COMMAND ${TCL_TCLSH}

        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE REQ_SQLITE3_OUTPUT

        ERROR_VARIABLE REQ_SQLITE3_ERROR
    )

    # Empty string means the `package require` had no error
    string(COMPARE EQUAL "${REQ_SQLITE3_ERROR}" "" TCL_SQLITE3_FOUND_)

    if (TCL_SQLITE3_FOUND_)
        set(TCL_SQLITE3_FOUND TRUE CACHE BOOL "" FORCE)
    endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(TclSQLite3
    REQUIRED_VARS TCLSH_FOUND TCL_SQLITE3_FOUND
    VERSION_VAR REQ_SQLITE3_OUTPUT
)
