#include <initializer_list>
#include <system_error>

#include <signal.h>
#include <sys/select.h>
#include <sys/signalfd.h>

#include "program_stoppable_sleep.hpp"
#include "signal_set.hpp"

class program_stop_detect_engine {
private:
    signal_set const signals{SIGINT, SIGTERM};
    signal_set restore;

    program_stop_detect_engine()
    {
        sigprocmask(SIG_BLOCK, &signals.handle, &restore.handle);
    }

    ~program_stop_detect_engine()
    {
        sigprocmask(SIG_UNBLOCK, &restore.handle, nullptr);
    }

public:
    static program_stop_detect_engine const& instance()
    {
        static auto self = program_stop_detect_engine{};
        return self;
    }

    int signalfd() const
    {
        int const fd = ::signalfd(-1, &signals.handle, {});
        if (fd == -1) {
            auto const signalfd_error = errno;
            throw std::system_error{
                    signalfd_error, std::generic_category(), "Couldn't open signalfd."};
        }
        return fd;
    }
};

program_stoppable_sleep::program_stoppable_sleep()
    : signalfd{program_stop_detect_engine::instance().signalfd()}
{
}

void program_stoppable_sleep::close()
{
    if (signalfd != -1)
        ::close(signalfd);
}

sleep_result program_stoppable_sleep::sleep(std::chrono::milliseconds units)
{
    auto const ms = units.count();
    timeval timeout{ms / 1000, 1000 * (ms % 1000)};

    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(signalfd, &rdfs);

    auto const result = select(signalfd + 1, &rdfs, nullptr, nullptr, &timeout);
    if (result == -1) {
        auto const select_error = errno;
        throw std::system_error{
                select_error, std::generic_category(), "select() failed."};
    }

    return result == 0 ? sleep_result::slept : sleep_result::cancelled;
}
