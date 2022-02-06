cmake_minimum_required(VERSION 3.20)

project(cosmic_detector LANGUAGES CXX)

find_package(fmt REQUIRED)

add_executable(cosmic_poll main.cpp)
target_link_libraries(cosmic_poll PRIVATE fmt::fmt)
