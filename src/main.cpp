// IBM says that 256 MiB will have a bit flip every month

#include <cstddef>
#include <cstdio>

#include <sys/mman.h>

#include <fmt/core.h>

#include "cancellable_sleep.hpp"
#include "command_line.hpp"
#include "log_db_engine.hpp"
#include "memory_monitor.hpp"
#include "observation_logger.hpp"
#include "program_stoppable_sleep.hpp"

enum exit_code {
    success = 0,
    memory_alloc_error,
    exception,
    unknown_exception,
    parse_error,
};

int run(std::size_t memory_size, std::chrono::milliseconds polling_interval)
{
    void const* const memory =
            mmap(nullptr,                                  // addr
                 memory_size,                              // length
                 PROT_READ | PROT_WRITE,                   // prot
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_LOCKED, // flags
                 -1,                                       // fd
                 0                                         // offset
            );

    if (memory == (void*)-1) {
        auto const mmap_errno = errno;
        fmt::print(stderr, "mmap failed with error code: {}\n", strerror(mmap_errno));
        return exit_code::memory_alloc_error;
    }

    auto const mlock_status = mlock(memory, memory_size);
    if (mlock_status == -1) {
        auto const mlock_errno = errno;
        fmt::print(stderr, "mlock failed with error code: {}\n", strerror(mlock_errno));
        return exit_code::memory_alloc_error;
    }

    observation_logger logger{
            log_db_engine{"/var/lib/cosmic_poll/activity.sqlite"}, memory_size};

    cancellable_sleep csleep{program_stoppable_sleep{}};

    auto const result =
            monitor_memory(memory, memory_size, csleep, logger, polling_interval);
    fmt::print("\n");

    if (std::holds_alternative<flip_detected>(result)) {
        auto const flip = std::get<flip_detected>(result);
        fmt::print(
                "Anomaly detected at offset {:016x} value {:02x}\n", flip.offset,
                (unsigned char)flip.value);
        fmt::print("\a");
        std::fflush(stdout);
    } else if (std::holds_alternative<cancelled>(result)) {
        fmt::print("Stop signal received. Stopping..\n");
    }

    return exit_code::success;
}

int main(int argc, char** argv)
{
    try {
        auto const parse_result = parse_args(argc, argv);
        if (holds_alternative<no_options>(parse_result)) {
            auto const res = std::get<no_options>(parse_result);
            return res == no_options::help_requested ? exit_code::success
                                                     : exit_code::parse_error;
        } else if (holds_alternative<options>(parse_result)) {
            auto const opts = std::get<options>(parse_result);
            return run(opts.alloc_size, opts.check_interval);
        }
    } catch (std::exception const& exc) {
        fmt::print(stderr, "Fatal exception: {}\n", exc.what());
        return exit_code::exception;
    } catch (...) {
        fmt::print(stderr, "Fatal unknown exception.\n");
        return exit_code::unknown_exception;
    }
}
