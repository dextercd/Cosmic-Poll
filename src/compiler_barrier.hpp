#ifndef COSMIC_POLL_COMPILER_BARRIER_HPP
#define COSMIC_POLL_COMPILER_BARRIER_HPP

#define COSMIC_COMPILER_READ_BARRIER() asm("" : : : "memory")

#endif // header guard
