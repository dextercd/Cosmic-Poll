add_executable(read_barrier_test
    read_barrier.cpp
)

target_include_directories(read_barrier_test
    PRIVATE ${CosmicPoll_SOURCE_DIR}/src/cosmic_poll
)

target_link_libraries(read_barrier_test PRIVATE fmt::fmt)

# Force optimisations on for read_barrier_test since this test relies on
# seeing if the read barrier successfully prevented certain compiler
# optimisations.
target_compile_options(read_barrier_test PRIVATE -O3)

add_test(NAME "Read barrier" COMMAND read_barrier_test)


find_package(Tclsh)
find_package(TclExpect)
find_package(TclSQLite3)
find_package(GDB)

if (TCLSH_FOUND AND TCL_EXPECT_FOUND AND GDB_FOUND)
    add_test(NAME "End to end"
        COMMAND ${TCL_TCLSH} ${CMAKE_CURRENT_SOURCE_DIR}/end_to_end.tcl
            ${GDB_EXECUTABLE} $<TARGET_FILE:cosmic_poll>
    )
else()
    message(STATUS "Missing dependency for end to end test")
    message(STATUS "TCLSH_FOUND: ${TCLSH_FOUND}")
    message(STATUS "TCL_EXPECT_FOUND: ${TCL_EXPECT_FOUND}")
    message(STATUS "GDB_FOUND: ${GDB_FOUND}")
endif()

if (TCLSH_FOUND AND TCL_EXPECT_FOUND AND TCL_SQLITE3_FOUND AND GDB_FOUND)
    add_test(NAME "End to SQLite"
        COMMAND ${TCL_TCLSH} ${CMAKE_CURRENT_SOURCE_DIR}/end_to_sqlite.tcl
            ${GDB_EXECUTABLE} $<TARGET_FILE:cosmic_poll>
    )
else()
    message(STATUS "Missing dependency for end to sqlite test")
    message(STATUS "TCLSH_FOUND: ${TCLSH_FOUND}")
    message(STATUS "TCL_EXPECT_FOUND: ${TCL_EXPECT_FOUND}")
    message(STATUS "TCL_SQLITE3_FOUND: ${TCL_SQLITE3_FOUND}")
    message(STATUS "GDB_FOUND: ${GDB_FOUND}")
endif()
