cmake_minimum_required(VERSION 3.20)

project(cosmic_detector LANGUAGES CXX)

find_package(fmt REQUIRED)

add_executable(cosmic_poll
    main.cpp
    program_stoppable_sleep.cpp
)
target_link_libraries(cosmic_poll PRIVATE fmt::fmt)
target_compile_features(cosmic_poll PRIVATE cxx_std_20)
