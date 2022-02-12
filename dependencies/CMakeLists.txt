include(FetchContent)
include(ExternalProject)

# fmt
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG b6f4ceaed0a0a24ccf575fab6c56dd50ccf6f1a9 # 8.1.1
)

FetchContent_MakeAvailable(fmt)

# SQLite 3
FetchContent_Declare(SQLite
    URL https://www.sqlite.org/2022/sqlite-amalgamation-3370200.zip
    URL_HASH SHA3_256=7f535314ac30f1c7847df2a66a9e16a322f55dae6e83b264178cf02114cd0617
)

FetchContent_MakeAvailable(SQLite)

add_library(sqlite3
    ${sqlite_SOURCE_DIR}/sqlite3.c
)
add_library(SQLite::SQLite3 ALIAS sqlite3)

target_include_directories(sqlite3
    INTERFACE ${sqlite_SOURCE_DIR}
)

target_compile_definitions(sqlite3
    PUBLIC
        SQLITE_OMIT_DEPRECATED
        SQLITE_OMIT_COMPILEOPTION_DIAGS

    PRIVATE
        SQLITE_THREADSAFE=0
        SQLITE_OMIT_LOAD_EXTENSION
)
