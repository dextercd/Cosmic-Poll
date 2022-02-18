#ifndef COSMIC_POLL_CREATE_LOG_DB_HPP
#define COSMIC_POLL_CREATE_LOG_DB_HPP

#include <copo/sqlite/sqlite_connection.hpp>

namespace copo {

void create_log_db(sqlite_connection& connection);

}

#endif // header guard
