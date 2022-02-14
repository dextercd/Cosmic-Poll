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
endif()

add_library(sqlite_wrapper
    src/sqlite_connection.cpp
    src/sqlite_statement.cpp
)

target_link_libraries(sqlite_wrapper
    PRIVATE
        SQLite::SQLite3
)

set_property(SOURCE src/compile_paths.cpp
    APPEND
    PROPERTY COMPILE_DEFINITIONS
        "COSMIC_DEFAULT_DB_LOCATION=\"${COSMIC_DEFAULT_DB_LOCATION}\""
)

add_executable(cosmic_poll
    src/main.cpp
    src/program_stoppable_sleep.cpp
    src/observation_logger.cpp
    src/log_db_engine.cpp
    src/create_log_db.cpp
    src/memory_monitor.cpp
    src/command_line.cpp
    src/compile_paths.cpp
)

target_link_libraries(cosmic_poll
    PRIVATE
        fmt::fmt
        sqlite_wrapper
        CLI11::CLI11
)

target_compile_features(cosmic_poll PRIVATE cxx_std_20)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
