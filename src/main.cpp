// IBM says that 256 MiB will have a bit flip every month

#include <cstddef>
#include <cstdio>

#include <sys/mman.h>

#include <fmt/core.h>

#include "cancellable_sleep.hpp"
#include "log_db_engine.hpp"
#include "memory_monitor.hpp"
#include "observation_logger.hpp"
#include "program_stoppable_sleep.hpp"

constexpr std::size_t memory_size = 256 * 1024 * 1024;

int run()
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
        fmt::print("mmap failed with error code: {}\n", strerror(mmap_errno));
        return 1;
    }

    auto const mlock_status = mlock(memory, memory_size);
    if (mlock_status == -1) {
        auto const mlock_errno = errno;
        fmt::print("mlock failed with error code: {}\n", strerror(mlock_errno));
        return 2;
    }

    observation_logger logger{
            log_db_engine{"/var/lib/cosmic_poll/activity.sqlite"}, memory_size};

    cancellable_sleep csleep{program_stoppable_sleep{}};

    auto const result = monitor_memory(memory, memory_size, csleep, logger);
    fmt::print("\n");

    if (std::holds_alternative<flip_detected>(result)) {
        auto const flip = std::get<flip_detected>(result);
        fmt::print(
                "Anomaly detected at offset {:016x} value {:02x}\n", flip.offset,
                flip.value);
        fmt::print("\a");
        std::fflush(stdout);
    } else if (std::holds_alternative<cancelled>(result)) {
        fmt::print("Stop signal received. Stopping..\n");
    }

    return 0;
}

int main()
{
    try {
        return run();
    } catch (std::exception const& exc) {
        fmt::print(stderr, "Fatal exception: {}\n", exc.what());
        return 1;
    } catch (...) {
        fmt::print(stderr, "Fatal unknown exception.\n");
    }
}
