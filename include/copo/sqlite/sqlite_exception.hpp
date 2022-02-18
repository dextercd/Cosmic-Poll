#ifndef COSMIC_POLL_SQLITE_EXCEPTION_HPP
#define COSMIC_POLL_SQLITE_EXCEPTION_HPP

#include <stdexcept>

namespace copo {

struct sqlite_exception : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct sqlite_connection_failure : sqlite_exception {
    using sqlite_exception::sqlite_exception;
};

struct sqlite_statement_error : sqlite_exception {
    using sqlite_exception::sqlite_exception;
};

struct sqlite_bind_error : sqlite_statement_error {
    using sqlite_statement_error::sqlite_statement_error;
};

} // namespace copo

#endif // header guard
