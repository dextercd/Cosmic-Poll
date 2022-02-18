#ifndef COSMIC_POLL_CANCELLABLE_SLEEP_HPP
#define COSMIC_POLL_CANCELLABLE_SLEEP_HPP

#include <chrono>
#include <memory>

namespace copo {

enum class sleep_result {
    slept,
    cancelled,
};

class cancellable_sleep {
    class cancellable_sleep_impl {
    public:
        virtual sleep_result sleep(std::chrono::milliseconds duration) = 0;
        virtual ~cancellable_sleep_impl() = default;
    };

    std::unique_ptr<cancellable_sleep_impl> impl;

public:
    template<class T>
    cancellable_sleep(T implementation)
    {
        struct wrapper : public cancellable_sleep_impl {
            T implementation;

            wrapper(T&& impl)
                : implementation{std::move(impl)}
            {
            }

            sleep_result sleep(std::chrono::milliseconds duration) override
            {
                return implementation.sleep(duration);
            }
        };

        impl = std::make_unique<wrapper>(std::move(implementation));
    }

    template<class Rep, class Period>
    sleep_result sleep(std::chrono::duration<Rep, Period> duration)
    {
        auto const units =
                std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        return impl->sleep(units);
    }
};

} // namespace copo

#endif // header guard
