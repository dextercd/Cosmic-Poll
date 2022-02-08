#ifndef COSMIC_POLL_SQLITE_STATEMENT_HPP
#define COSMIC_POLL_SQLITE_STATEMENT_HPP

#include <cstring> // std::size_t

#include "sqlite_exception.hpp"

struct sqlite3_stmt;
class sqlite_connection;

enum class step_result {
    busy,
    done,
    row,
};

inline char const* to_char_star(step_result result)
{
    switch (result) {
    case step_result::busy:
        return "step_result::busy";
    case step_result::done:
        return "step_result::done";
    case step_result::row:
        return "step_result::row";
    default:
        return "(unknown)";
    }
}

class sqlite_statement {
private:
    sqlite3_stmt* handle;

public:
    sqlite_statement(sqlite_connection& connection, char const* sql);
    sqlite_statement(sqlite_statement&&) = delete;
    ~sqlite_statement();

    void bind(int ix, double value);
    void bind(int ix, char const* value);
    void bind(int ix, std::size_t value);

    void bind(int ix, unsigned value)
    {
        return bind(ix, (std::size_t)value);
    }

    step_result step();

    void step_done()
    {
        auto const result = step();
        if (result != step_result::done) {
            throw sqlite_statement_error{
                    std::string{"Expected step() to return step_result::done but instead "
                                "got: "} +
                    to_char_star(result)};
        }
    }

    int column_count();
};

#endif // header guard
