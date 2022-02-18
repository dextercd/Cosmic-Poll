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

int run(copo::options const& opts)
{
    void const* const memory =
            mmap(nullptr,                                  // addr
                 opts.alloc_size,                          // length
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

    auto const mlock_status = mlock(memory, opts.alloc_size);
    if (mlock_status == -1) {
        auto const mlock_errno = errno;
        fmt::print(stderr, "mlock failed with error code: {}\n", strerror(mlock_errno));
        return exit_code::memory_alloc_error;
    }

    copo::observation_logger logger{
            copo::log_db_engine{opts.db_location.c_str()}, opts.alloc_size};

    copo::cancellable_sleep csleep{copo::program_stoppable_sleep{}};

    auto const result = copo::monitor_memory(
            memory, opts.alloc_size, csleep, logger, opts.check_interval);
    fmt::print("\n");

    if (std::holds_alternative<copo::flip_detected>(result)) {
        auto const flip = std::get<copo::flip_detected>(result);
        fmt::print(
                "Anomaly detected at offset {:016x} value {:02x}\n", flip.offset,
                flip.value);
        fmt::print("\a");
        std::fflush(stdout);
    } else if (std::holds_alternative<copo::cancelled>(result)) {
        fmt::print("Stop signal received. Stopping..\n");
    }

    return exit_code::success;
}

int main(int argc, char** argv)
{
    try {
        auto const parse_result = copo::parse_args(argc, argv);
        if (holds_alternative<copo::no_options>(parse_result)) {
            auto const res = std::get<copo::no_options>(parse_result);
            return res == copo::no_options::help_requested ? exit_code::success
                                                           : exit_code::parse_error;
        } else if (holds_alternative<copo::options>(parse_result)) {
            auto const opts = std::get<copo::options>(parse_result);
            return run(opts);
        }
    } catch (std::exception const& exc) {
        fmt::print(stderr, "Fatal exception: {}\n", exc.what());
        return exit_code::exception;
    } catch (...) {
        fmt::print(stderr, "Fatal unknown exception.\n");
        return exit_code::unknown_exception;
    }
}
