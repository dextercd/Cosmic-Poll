// IBM says that 256 MiB will have a bit flip every month

#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>
#include <variant>

#include <sys/mman.h>

#include <fmt/core.h>

#include "cancellable_sleep.hpp"
#include "program_stoppable_sleep.hpp"
#include "compiler_barrier.hpp"

auto const polling_time = std::chrono::seconds{15};

struct flip_detected {
    std::ptrdiff_t offset;
    char value;
};

struct cancelled {};

using monitor_result = std::variant<flip_detected, cancelled>;

monitor_result monitor_memory(
        void const* const memory,
        std::size_t const size,
        cancellable_sleep& csleep)
{
    auto const begin = reinterpret_cast<char const*>(memory);
    auto const end = begin + size;
    while (true) {
        fmt::print(".");
        std::fflush(stdout);

        COSMIC_COMPILER_READ_BARRIER();

        for (auto ptr = begin; ptr != end; ++ptr) {
            if (*ptr != 0) {
                auto const offset = ptr - begin;
                return flip_detected{offset, *ptr};
            }
        }

        if (csleep.sleep(polling_time) == sleep_result::cancelled)
            return cancelled{};
    }
}

constexpr std::size_t memory_size = 256 * 1024 * 1024;

int main()
{
    void const* const memory = mmap(
        nullptr,                                  // addr
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

    cancellable_sleep csleep{program_stoppable_sleep{}};

    auto const result = monitor_memory(memory, memory_size, csleep);
    fmt::print("\n");

    if (std::holds_alternative<flip_detected>(result)) {
        auto const flip = std::get<flip_detected>(result);
        fmt::print("Anomaly detected at offset {:016x} value {:02x}\n", flip.offset, flip.value);
        while (true) {
            fmt::print("\a");
            std::fflush(stdout);
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }
    } else if (std::holds_alternative<cancelled>(result)) {
        fmt::print("Stop signal received. Stopping..\n");
        return 0;
    }
}
