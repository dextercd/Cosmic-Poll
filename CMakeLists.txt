cmake_minimum_required(VERSION 3.20)

option(COSMIC_SELF_CONTAINED "Build dependencies instead of looking for them" OFF)
set(COSMIC_DEFAULT_DB_LOCATION "/var/lib/cosmic_poll/activity.sqlite"
    CACHE FILEPATH
    "Location used for database when none is specified")

set(LANGUAGES CXX)
if (COSMIC_SELF_CONTAINED)
    # Needed to build C dependencies (SQLite)
    list(APPEND LANGUAGES C)
endif()

project(CosmicPoll LANGUAGES ${LANGUAGES})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CTest)

if (COSMIC_SELF_CONTAINED)
    add_subdirectory(dependencies)
else()
    find_package(fmt REQUIRED)
    find_package(SQLite3 REQUIRED)
    find_package(CLI11 REQUIRED)
    find_package(TBB REQUIRED COMPONENTS tbb)
endif()

add_subdirectory(src)

set_property(
    SOURCE src/cosmic_poll/compile_paths.cpp TARGET_DIRECTORY cosmic_poll
    APPEND
    PROPERTY COMPILE_DEFINITIONS
        "COSMIC_DEFAULT_DB_LOCATION=\"${COSMIC_DEFAULT_DB_LOCATION}\""
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
