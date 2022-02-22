#include <chrono>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include "create_log_db.hpp"
#include <copo/sqlite/sqlite_statement.hpp>

#include "log_db_engine.hpp"

namespace copo {

log_db_engine::log_db_engine(char const* filename)
    : connection{filename}
{
    create_log_db(connection);
}

void log_db_engine::log_active_period(
        observation::duration duration, observation::time_point now,
        std::size_t byte_count, unsigned char mask)
{
    auto const seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();

    auto statement = sqlite_statement{connection, R"(
            INSERT INTO active_times(seconds, time_submitted, bytes, mask)
                VALUES(?, ?, ?, ?);
            )"};

    statement.bind(1, seconds);
    statement.bind(2, fmt::format("{}", now).c_str());
    statement.bind(3, byte_count);
    statement.bind(4, (unsigned)mask);
    statement.step_done();
}

void log_db_engine::log_anomaly(
        observation::time_point const now, std::size_t const offset,
        unsigned char const value, std::size_t const byte_count, unsigned char mask)
{
    auto statement = sqlite_statement{connection, R"(
            INSERT INTO anomalies(time_submitted, offset, value, bytes, mask)
                VALUES(?, ?, ?, ?, ?);
            )"};

    statement.bind(1, fmt::format("{}", now).c_str());
    statement.bind(2, offset);
    statement.bind(3, (unsigned)value);
    statement.bind(4, byte_count);
    statement.bind(5, (unsigned)mask);
    statement.step_done();
}

} // namespace copo
