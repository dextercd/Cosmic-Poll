#ifndef COSMIC_COMMAND_LINE_HPP
#define COSMIC_COMMAND_LINE_HPP

#include <chrono>
#include <variant>

struct options {
    std::uint64_t alloc_size;
    std::chrono::milliseconds check_interval;
};

enum class no_options {
    help_requested,
    parse_error,
};

using parse_result = std::variant<options, no_options>;

parse_result parse_args(int argc, char const* const* argv);

#endif // header guard
