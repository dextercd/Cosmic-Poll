#include <stdexcept>

#include "sqlite_connection.hpp"
#include "sqlite_statement.hpp"

namespace copo {

namespace {

bool table_exists(sqlite_connection& connection, char const* const table)
{
    auto statement = sqlite_statement{connection, R"(
        SELECT 1
        FROM sqlite_master
        WHERE type='table' AND name=?;
    )"};

    statement.bind(1, table);
    switch (statement.step()) {
    case step_result::row:
        return true;
    case step_result::done:
        return false;
    default:
        throw std::runtime_error{"Unexpected step result."};
    }
}

int get_db_revision(sqlite_connection& connection)
{
    if (!table_exists(connection, "active_times"))
        return 0;

    if (!table_exists(connection, "anomalies"))
        return 1;

    return 2;
}

} // namespace

void create_log_db(sqlite_connection& connection)
{
    auto const initial_version = get_db_revision(connection);

    if (initial_version < 1) {
        auto statement = sqlite_statement{connection, R"(
            CREATE TABLE active_times(
                seconds REAL,
                time_submitted TEXT,
                bytes INTEGER
            ) STRICT;)"};

        statement.step_done();
    }

    if (initial_version < 2) {
        auto statement = sqlite_statement{connection, R"(
            CREATE TABLE anomalies(
                time_submitted TEXT,
                offset INTEGER,
                value INTEGER,
                bytes INTEGER
            ) STRICT;)"};

        statement.step_done();
    }
}

} // namespace copo
