#include <stdexcept>

#include <copo/sqlite/sqlite_connection.hpp>
#include <copo/sqlite/sqlite_statement.hpp>

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

bool column_exists(sqlite_connection& conn, char const* table, char const* column)
{
    auto statement = sqlite_statement{conn, R"(
        SELECT 1
        FROM pragma_table_info(?)
        WHERE name=?;
    )"};

    statement.bind(1, table);
    statement.bind(2, column);

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

    if (!column_exists(connection, "active_times", "mask"))
        return 2;

    if (!column_exists(connection, "anomalies", "mask"))
        return 3;

    return 4;
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
            );)"};

        statement.step_done();
    }

    if (initial_version < 2) {
        auto statement = sqlite_statement{connection, R"(
            CREATE TABLE anomalies(
                time_submitted TEXT,
                offset INTEGER,
                value INTEGER,
                bytes INTEGER
            );)"};

        statement.step_done();
    }

    if (initial_version < 3) {
        auto statement1 = sqlite_statement{connection, R"(
            ALTER TABLE active_times ADD COLUMN mask INTEGER;
        )"};

        statement1.step_done();

        auto statement2 = sqlite_statement{connection, R"(
            UPDATE active_times SET mask=0 WHERE mask IS NULL;
        )"};

        statement2.step_done();
    }

    if (initial_version < 4) {
        auto statement1 = sqlite_statement{connection, R"(
            ALTER TABLE anomalies ADD COLUMN mask INTEGER;
        )"};

        statement1.step_done();

        auto statement2 = sqlite_statement{connection, R"(
            UPDATE anomalies SET mask=0 WHERE mask IS NULL;
        )"};

        statement2.step_done();
    }
}

} // namespace copo
