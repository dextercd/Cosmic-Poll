#include <sqlite3.h>

#include <copo/sqlite/sqlite_connection.hpp>
#include <copo/sqlite/sqlite_exception.hpp>

namespace copo {

sqlite_connection::sqlite_connection(char const* const filename)
{
    auto const result = sqlite3_open_v2(
            filename, &handle,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_EXRESCODE, nullptr);

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

} // namespace copo
