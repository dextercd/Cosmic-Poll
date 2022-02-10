#ifndef COSMIC_POLL_MEMORY_MONITOR_HPP
#define COSMIC_POLL_MEMORY_MONITOR_HPP

#include <cstddef>
#include <variant>

#include "cancellable_sleep.hpp"
#include "observation_logger.hpp"

struct flip_detected {
    std::ptrdiff_t offset;
    char value;
};

struct cancelled {};

using monitor_result = std::variant<flip_detected, cancelled>;

monitor_result monitor_memory(
        void const* const memory, std::size_t const size, cancellable_sleep& csleep,
        observation_logger& logger);

#endif // header guard