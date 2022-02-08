#ifndef COSMIC_POLL_OBSERVATION_TYPES_HPP
#define COSMIC_POLL_OBSERVATION_TYPES_HPP

#include <chrono>

namespace observation {

using duration = std::chrono::milliseconds;
using clock = std::chrono::system_clock;
using time_point = std::chrono::sys_time<duration>;

} // namespace observation

#endif // header guard
