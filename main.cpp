// IBM says that 256 MiB will have a bit flip every month

#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>
#include <variant>

#include <fmt/core.h>

#include <sys/mman.h>

auto const polling_time = std::chrono::seconds{15};

struct flip_detected {
    std::ptrdiff_t offset;
    char value;
};

using monitor_result = std::variant<flip_detected>;

monitor_result monitor_memory(
        void const* const memory,
        std::size_t const size)
{
    auto const begin = reinterpret_cast<char const*>(memory);
    auto const end = begin + size;
    while (true) {
        fmt::print(".");
        std::fflush(stdout);
        for (auto ptr = begin; ptr != end; ++ptr) {
            if (*ptr != 0) {
                auto const offset = ptr - begin;
                return flip_detected{offset, *ptr};
            }
        }

        std::this_thread::sleep_for(polling_time);
    }
}

constexpr std::size_t memory_size = 256 * 1024 * 1024;

int main()
{
    void const* const memory = mmap(
        nullptr,                                  // addr
        memory_size,                              // length
        PROT_READ,                                // prot
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

    auto const result = monitor_memory(memory, memory_size);

    if (std::holds_alternative<flip_detected>(result)) {
        auto const flip = std::get<flip_detected>(result);
        fmt::print("\nAnomaly detected at offset {:016x} value {:02x}\n", flip.offset, flip.value);
        while (true) {
            fmt::print("\a");
            std::fflush(stdout);
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }
    }
}
