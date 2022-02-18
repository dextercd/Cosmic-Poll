#include <cstring>

#include <sqlite3.h>

#include <copo/sqlite/sqlite_connection.hpp>
#include <copo/sqlite/sqlite_exception.hpp>

#include <copo/sqlite/sqlite_statement.hpp>

namespace copo {

sqlite_statement::sqlite_statement(sqlite_connection& connection, char const* const sql)
{
    auto const result = sqlite3_prepare_v3(
            connection.handle, sql, std::strlen(sql), {}, &handle, nullptr);

    if (result != SQLITE_OK)
        throw sqlite_statement_error(sqlite3_errmsg(connection.handle));
}

sqlite_statement::~sqlite_statement()
{
    sqlite3_finalize(handle);
}

void sqlite_statement::bind(int const ix, double const value)
{
    auto const result = sqlite3_bind_double(handle, ix, value);
    if (result != SQLITE_OK)
        throw sqlite_bind_error(sqlite3_errmsg(sqlite3_db_handle(handle)));
}

void sqlite_statement::bind(int const ix, char const* const value)
{
    auto const result =
            sqlite3_bind_text(handle, ix, value, std::strlen(value), SQLITE_TRANSIENT);
    if (result != SQLITE_OK)
        throw sqlite_bind_error(sqlite3_errmsg(sqlite3_db_handle(handle)));
}

void sqlite_statement::bind(int const ix, std::size_t const value)
{
    auto const result = sqlite3_bind_int64(handle, ix, value);
    if (result != SQLITE_OK)
        throw sqlite_bind_error(sqlite3_errmsg(sqlite3_db_handle(handle)));
}

step_result sqlite_statement::step()
{
    switch (sqlite3_step(handle)) {
    case SQLITE_BUSY:
        return step_result::busy;
    case SQLITE_DONE:
        return step_result::done;
    case SQLITE_ROW:
        return step_result::row;

    default:
        throw sqlite_statement_error(sqlite3_errmsg(sqlite3_db_handle(handle)));
    }
}

int sqlite_statement::column_count()
{
    return sqlite3_column_count(handle);
}

} // namespace copo
