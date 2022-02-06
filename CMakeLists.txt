cmake_minimum_required(VERSION 3.20)

project(cosmic_detector LANGUAGES CXX)

include(CTest)

find_package(fmt REQUIRED)

add_executable(cosmic_poll
    src/main.cpp
    src/program_stoppable_sleep.cpp
)
target_link_libraries(cosmic_poll PRIVATE fmt::fmt)
target_compile_features(cosmic_poll PRIVATE cxx_std_20)

if (BUILD_TESTING)
    add_executable(read_barrier_test
        tests/read_barrier.cpp
    )

    target_include_directories(read_barrier_test
        PRIVATE src/
    )

    target_link_libraries(read_barrier_test PRIVATE fmt::fmt)

    # Force optimisations on for read_barrier_test since this test relies on
    # seeing if the read barrier successfully prevented certain compiler
    # optimisations.
    target_compile_options(read_barrier_test PRIVATE -O3)

    add_test(NAME "Read barrier" COMMAND read_barrier_test)
endif()
