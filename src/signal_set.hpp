#ifndef COSMIC_POLL_SIGNAL_SET_HPP
#define COSMIC_POLL_SIGNAL_SET_HPP

#include <initializer_list>
#include <system_error>

#include <errno.h>
#include <signal.h>

namespace copo {

struct signal_set {
    sigset_t handle;

    signal_set()
    {
        auto const result = sigemptyset(&handle);
        if (result == -1) {
            auto const sigemptyset_error = errno;
            throw std::system_error{
                    sigemptyset_error, std::generic_category(),
                    "Couldn't create signal set."};
        }
    }

    signal_set(std::initializer_list<int> signals)
        : signal_set{}
    {
        add(signals);
    }

    void add(int signum)
    {
        auto const result = sigaddset(&handle, signum);
        if (result == -1) {
            auto const sigaddset_error = errno;
            throw std::system_error{
                    sigaddset_error, std::generic_category(),
                    "Couldn't add signal number to signal set."};
        }
    }

    void add(std::initializer_list<int> signals)
    {
        for (auto const signal : signals)
            add(signal);
    }
};

} // namespace copo

#endif // header guard
