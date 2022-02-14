#include <chrono>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include "create_log_db.hpp"
#include "sqlite_statement.hpp"

#include "log_db_engine.hpp"

log_db_engine::log_db_engine(char const* filename)
    : connection{filename}
{
    create_log_db(connection);
}

void log_db_engine::log_active_period(
        observation::duration duration, observation::time_point now,
        std::size_t byte_count)
{
    auto const seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();

    auto statement = sqlite_statement{connection, R"(
            INSERT INTO active_times(seconds, time_submitted, bytes)
                VALUES(?, ?, ?);
            )"};

    statement.bind(1, seconds);
    statement.bind(2, fmt::format("{}", now).c_str());
    statement.bind(3, byte_count);
    statement.step_done();
}

void log_db_engine::log_anomaly(
        observation::time_point const now, std::size_t const offset,
        unsigned char const value, std::size_t const byte_count)
{
    auto statement = sqlite_statement{connection, R"(
            INSERT INTO anomalies(time_submitted, offset, value, bytes)
                VALUES(?, ?, ?, ?);
            )"};

    statement.bind(1, fmt::format("{}", now).c_str());
    statement.bind(2, offset);
    statement.bind(3, (unsigned)value);
    statement.bind(4, byte_count);
    statement.step_done();
}
