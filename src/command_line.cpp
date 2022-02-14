#include <chrono>
#include <cstdint>
#include <filesystem>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>

#include "command_line.hpp"
#include "compile_paths.hpp"

std::map<std::string, std::uint64_t> create_duration_mapping()
{
    auto const seconds = std::uint64_t{1000};
    auto const minutes = seconds * 60;
    auto const hours = minutes * 60;
    auto const days = hours * 24;

    // clang-format off
    auto const duration_mapping = std::map<std::string, std::uint64_t>{
        {"ms", 1}, {"millisecond", 1}, {"milliseconds", 1},
        {"s", seconds}, {"sec", seconds}, {"second", seconds}, {"seconds", seconds},
        {"m", minutes}, {"min", minutes}, {"minute", minutes}, {"minutes", minutes},
        {"h", hours}, {"hr", hours}, {"hour", hours}, {"hours", hours},
        {"day", days}, {"days", days},
    };
    // clang-format on

    return duration_mapping;
}

parse_result parse_args(int argc, char const* const* argv)
{
    std::uint64_t alloc_size = 256 * 1024 * 1024;
    std::chrono::milliseconds check_interval = std::chrono::minutes{5};
    std::filesystem::path db_location = default_db_location;

    CLI::App app{
            "Allocate some memory and periodically check it for cosmic ray bit flips."};

    app.add_option("--alloc-size", alloc_size)->transform(CLI::AsSizeValue(true));

    auto const duration_transform = CLI::AsNumberWithUnit(create_duration_mapping());
    app.add_option_function<std::uint64_t>(
               "--check-interval",
               [&](std::uint64_t v) { check_interval = std::chrono::milliseconds{v}; })
            ->option_text("<number> <unit (seconds, minutes, ..)>")
            ->transform(duration_transform);

    app.add_option("--db-location", db_location)->capture_default_str();

    try {
        app.parse(argc, argv);
    } catch (CLI::Error const& err) {
        app.exit(err);

        if (err.get_exit_code() == 0)
            return no_options::help_requested;

        return no_options::parse_error;
    }

    return options{alloc_size, check_interval, db_location};
}
