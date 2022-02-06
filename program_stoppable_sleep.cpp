#include <initializer_list>
#include <stdexcept>

#include <signal.h>
#include <sys/signalfd.h>
#include <sys/select.h>

#include "program_stoppable_sleep.hpp"

struct signal_set {
    sigset_t handle;

    signal_set()
    {
        auto const result = sigemptyset(&handle);
        if (result == -1)
            throw std::runtime_error{"Couldn't create signal set."};
    }

    signal_set(std::initializer_list<int> signals)
        : signal_set{}
    {
        add(signals);
    }

    void add(int signum)
    {
        auto const result = sigaddset(&handle, signum);
        if (result == -1)
            throw std::runtime_error{"Couldn't add signal number to signal set."};
    }

    void add(std::initializer_list<int> signals)
    {
        for (auto const signal : signals)
            add(signal);
    }
};

class program_stop_detect_engine {
private:
    signal_set const signals{SIGINT, SIGTERM};
    signal_set restore;
private:
    program_stop_detect_engine()
    {
        sigprocmask(SIG_BLOCK, &signals.handle, &restore.handle);
    }

    ~program_stop_detect_engine()
    {
        sigprocmask(SIG_UNBLOCK, &restore.handle, nullptr);
    }

public:
    static program_stop_detect_engine const& instance() {
        static auto self = program_stop_detect_engine{};
        return self;
    }

    int signalfd() const
    {
        int const fd = ::signalfd(-1, &signals.handle, {});
        if (fd == -1)
            throw std::runtime_error{"Couldn't open signalfd."};
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
    timeval timeout{
        ms / 1000,
        1000 * (ms % 1000)
    };

    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(signalfd, &rdfs);

    while (true) {
        auto const result = select(signalfd + 1, &rdfs, nullptr, nullptr, &timeout);
        if (result == -1) {
            auto const select_error = errno;
            throw std::runtime_error{"select() failed. " + std::to_string(select_error)};
        }

        return result == 0 ? sleep_result::slept : sleep_result::cancelled;
    }
}
