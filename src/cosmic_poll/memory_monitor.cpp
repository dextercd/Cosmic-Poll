#include <algorithm>
#include <cstddef>
#include <execution>

#include <fmt/core.h>

#include "compiler_barrier.hpp"
#include "memory_monitor.hpp"

namespace copo {

monitor_result monitor_memory(
        void const* const memory, std::size_t const size, cancellable_sleep& csleep,
        observation_logger& logger, std::chrono::milliseconds polling_interval, unsigned char mask)
{
    auto const begin = reinterpret_cast<unsigned char const*>(memory);
    auto const end = begin + size;
    while (true) {
        fmt::print(".");
        std::fflush(stdout);

        logger.active();

        COSMIC_COMPILER_READ_BARRIER();

        auto ptr = std::find_if(
                std::execution::unseq, begin, end, [=](auto v) { return v != mask; });

        if (ptr != end) {
            auto const offset = ptr - begin;
            auto const value = static_cast<unsigned char>(*ptr ^ mask);

            logger.found_anomaly(offset, value);
            return flip_detected{offset, value};
        }

        if (csleep.sleep(polling_interval) == sleep_result::cancelled)
            return cancelled{};
    }
}

} // namespace copo
