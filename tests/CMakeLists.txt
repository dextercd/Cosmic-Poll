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
