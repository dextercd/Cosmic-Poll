#ifndef COSMIC_POLL_LOG_DB_ENGINE_HPP
#define COSMIC_POLL_LOG_DB_ENGINE_HPP

#include <cstddef>

#include "observation_types.hpp"
#include "sqlite_connection.hpp"

class log_db_engine {
private:
    sqlite_connection connection;

public:
    log_db_engine(char const* filename);

    void log_active_period(
            observation::duration duration, observation::time_point now,
            std::size_t byte_count);

    void log_anomaly(
            observation::time_point now, std::size_t offset, unsigned char value,
            std::size_t byte_count);
};

#endif // header guard
