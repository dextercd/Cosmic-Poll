add_executable(read_barrier_test
    read_barrier.cpp
)

target_include_directories(read_barrier_test
    PRIVATE ${CosmicPoll_SOURCE_DIR}/src/
)

target_link_libraries(read_barrier_test PRIVATE fmt::fmt)

# Force optimisations on for read_barrier_test since this test relies on
# seeing if the read barrier successfully prevented certain compiler
# optimisations.
target_compile_options(read_barrier_test PRIVATE -O3)

add_test(NAME "Read barrier" COMMAND read_barrier_test)


find_package(Tclsh)
find_package(GDB)

if (TCLSH_FOUND)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E echo "package require Expect"
        COMMAND ${TCL_TCLSH}
        OUTPUT_VARIABLE REQ_EXPECT_OUTPUT
        ERROR_VARIABLE REQ_EXPECT_OUTPUT
    )

    # Empty string means the `package require` had no error
    string(COMPARE EQUAL "${REQ_EXPECT_OUTPUT}" "" TCLEXPECT_FOUND)
endif()

if (TCLSH_FOUND AND TCLEXPECT_FOUND AND GDB_FOUND)
    add_test(NAME "End to end"
        COMMAND ${TCL_TCLSH} ${CMAKE_CURRENT_SOURCE_DIR}/end_to_end.tcl
            ${GDB_EXECUTABLE} $<TARGET_FILE:cosmic_poll>
    )
else()
    message(STATUS "Missing dependency for end to end test")
    message(STATUS "TCLSH_FOUND: ${TCLSH_FOUND}")
    message(STATUS "TCLEXPECT_FOUND: ${TCLEXPECT_FOUND}")
    message(STATUS "GDB_FOUND: ${GDB_FOUND}")
endif()
