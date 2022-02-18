#ifndef COSMIC_POLL_PROGRAM_STOPPABLE_SLEEP_HPP
#define COSMIC_POLL_PROGRAM_STOPPABLE_SLEEP_HPP

#include <chrono>

#include "cancellable_sleep.hpp"

namespace copo {

class program_stoppable_sleep {
private:
    int signalfd = -1;

    void close();

public:
    program_stoppable_sleep();
    ~program_stoppable_sleep()
    {
        close();
    }

    program_stoppable_sleep(program_stoppable_sleep&& other)
        : signalfd{other.signalfd}
    {
        other.signalfd = -1;
    }

    program_stoppable_sleep& operator=(program_stoppable_sleep&& other)
    {
        close();
        signalfd = other.signalfd;
        other.signalfd = -1;
        return *this;
    }

    sleep_result sleep(std::chrono::milliseconds units);
};

} // namespace copo

#endif // header guard
