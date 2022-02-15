find_package(Tclsh)

set(TCL_EXPECT_FOUND FALSE CACHE BOOL "Assume TclExpect is found")

if (TCLSH_FOUND)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo
            "package require Expect; puts [exp_version]"
        COMMAND ${TCL_TCLSH}

        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE REQ_EXPECT_OUTPUT

        ERROR_VARIABLE REQ_EXPECT_ERROR
    )

    # Empty string means the `package require` had no error
    string(COMPARE EQUAL "${REQ_EXPECT_ERROR}" "" TCL_EXPECT_FOUND_)

    if (TCL_EXPECT_FOUND_)
        set(TCL_EXPECT_FOUND TRUE CACHE BOOL "" FORCE)
    endif()
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(TclExpect
    REQUIRED_VARS TCLSH_FOUND TCL_EXPECT_FOUND
    VERSION_VAR REQ_EXPECT_OUTPUT
)
