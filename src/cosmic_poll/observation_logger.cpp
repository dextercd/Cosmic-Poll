#include <chrono>

#include "observation_logger.hpp"

namespace copo {

auto const log_every = std::chrono::minutes{30};

observation_logger::observation_logger(std::size_t const bc)
    : period_start{std::chrono::time_point_cast<observation::duration>(
              observation::clock::now())}
    , period_end{period_start}
    , byte_count{bc}
{
}

observation_logger::~observation_logger()
{
    dump_active();
}

void observation_logger::active()
{
    activity_in_period = true;
    period_end = std::chrono::time_point_cast<observation::duration>(
            observation::clock::now());

    auto const duration = period_end - period_start;
    if (duration >= log_every)
        dump_active();
}

void observation_logger::dump_active()
{
    if (activity_in_period) {
        activity_in_period = false;

        auto const duration = period_end - period_start;
        engine->log_active_period(duration, period_end, byte_count);

        period_start = period_end;
    }
}

void observation_logger::found_anomaly(
        std::size_t const offset, unsigned char const value)
{
    period_end = std::chrono::time_point_cast<observation::duration>(
            observation::clock::now());
    activity_in_period = true;

    engine->log_anomaly(period_end, offset, value, byte_count);
    dump_active();
}

} // namespace copo
