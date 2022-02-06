#include <cstdio>
#include <fmt/core.h>

#include <compiler_barrier.hpp>

[[gnu::section("without_read_barrier")]]
[[gnu::used]]
int without_read_barrier(int* ip)
{
    int a = *ip;
    int b = *ip;
    int c = *ip;
    int d = *ip;
    int e = *ip;

    return a + b + c + d + e;
}

[[gnu::section("with_read_barrier")]]
[[gnu::used]]
int with_read_barrier(int* ip)
{
    COSMIC_COMPILER_READ_BARRIER();
    int a = *ip;
    COSMIC_COMPILER_READ_BARRIER();
    int b = *ip;
    COSMIC_COMPILER_READ_BARRIER();
    int c = *ip;
    COSMIC_COMPILER_READ_BARRIER();
    int d = *ip;
    COSMIC_COMPILER_READ_BARRIER();
    int e = *ip;

    return a + b + c + d + e;
}

extern char const __start_without_read_barrier;
extern char const __stop_without_read_barrier;

extern char const __start_with_read_barrier;
extern char const __stop_with_read_barrier;

int main()
{
    auto without_barrier_size = &__stop_without_read_barrier - &__start_without_read_barrier;
    auto with_barrier_size = &__stop_with_read_barrier - &__start_with_read_barrier;

    if (with_barrier_size <= without_barrier_size) {
        fmt::print(stderr, "ERROR: The code with read barriers should be longer!\n");
        fmt::print(stderr, "Without read barriers: {} bytes\n", without_barrier_size);
        fmt::print(stderr, "With read barriers:    {} bytes\n", with_barrier_size);
        return 1;
    }

    return 0;
}
