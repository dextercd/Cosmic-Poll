#ifndef COSMIC_POLL_SQLITE_CONNECTION_HPP
#define COSMIC_POLL_SQLITE_CONNECTION_HPP

struct sqlite3;

namespace copo {

class sqlite_connection {
    sqlite3* handle;

public:
    explicit sqlite_connection(char const* filename);

    sqlite_connection(sqlite_connection&& other)
        : handle{other.handle}
    {
        other.handle = nullptr;
    }

    ~sqlite_connection();

    sqlite_connection& operator=(sqlite_connection&& other)
    {
        if (this != &other) {
            close_connection();
            handle = other.handle;
            other.handle = nullptr;
        }

        return *this;
    }

    bool is_connected() const
    {
        return handle != nullptr;
    }

    explicit operator bool() const
    {
        return is_connected();
    }

    void close_connection();

    friend class sqlite_statement;
};

} // namespace copo

#endif // header guard
