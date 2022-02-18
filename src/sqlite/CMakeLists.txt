add_library(sqlite_wrapper
    sqlite_connection.cpp
    sqlite_statement.cpp
)

target_link_libraries(sqlite_wrapper
    PRIVATE
        SQLite::SQLite3
)

target_include_directories(sqlite_wrapper
    PUBLIC
        ${CosmicPoll_SOURCE_DIR}/include
)
