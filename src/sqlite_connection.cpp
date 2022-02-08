#include <sqlite3.h>

#include "sqlite_connection.hpp"
#include "sqlite_exception.hpp"

sqlite_connection::sqlite_connection(char const* const filename)
{
    auto const result = sqlite3_open(filename, &handle);
    if (result != SQLITE_OK) {
        char const* error_message;
        if (handle) {
            error_message = sqlite3_errmsg(handle);
            close_connection();
        } else {
            error_message = sqlite3_errstr(result);
        }

        throw sqlite_connection_failure{error_message};
    }

    sqlite3_extended_result_codes(handle, true);
}

sqlite_connection::~sqlite_connection()
{
    sqlite3_close(handle);
}

void sqlite_connection::close_connection()
{
    sqlite3_close(handle);
    handle = nullptr;
}
