#ifndef COSMIC_POLL_OBSERVATION_LOGGER_HPP
#define COSMIC_POLL_OBSERVATION_LOGGER_HPP

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include "observation_types.hpp"

namespace copo {

class observation_logger {
    struct base_engine {
        virtual void log_active_period(
                observation::duration duration, observation::time_point now,
                std::size_t byte_count, unsigned char mask) = 0;
        virtual void log_anomaly(
                observation::time_point now, std::size_t offset, unsigned char value,
                std::size_t byte_count, unsigned char mask) = 0;
        virtual ~base_engine() = default;
    };

    std::unique_ptr<base_engine> engine;
    observation::time_point period_start;
    observation::time_point period_end;
    bool activity_in_period = false;
    std::size_t byte_count;
    unsigned char mask;

private:
    explicit observation_logger(std::size_t byte_count, unsigned char mask);

public:
    template<class Engine>
    observation_logger(Engine&& implementation, std::size_t byte_count, unsigned char mask)
        : observation_logger(byte_count, mask)
    {
        struct wrapper : base_engine {
            using engine_type = std::remove_cvref_t<Engine>;
            engine_type engine;

            wrapper(engine_type eng)
                : engine{std::move(eng)}
            {
            }

            void log_active_period(
                    observation::duration duration, observation::time_point now,
                    std::size_t byte_count, unsigned char mask) override
            {
                return engine.log_active_period(duration, now, byte_count, mask);
            }

            void log_anomaly(
                    observation::time_point now, std::size_t offset, unsigned char value,
                    std::size_t byte_count, unsigned char mask) override
            {
                return engine.log_anomaly(now, offset, value, byte_count, mask);
            }
        };

        engine = std::make_unique<wrapper>(std::forward<Engine>(implementation));
    }

    ~observation_logger();

    void active();
    void dump_active();
    void found_anomaly(std::size_t offset, unsigned char value);
};

} // namespace copo

#endif // header guard
