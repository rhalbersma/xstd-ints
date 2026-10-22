//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BENCH_ALTERNATIVES_HPP
#define XSTD_INTS_BENCH_ALTERNATIVES_HPP

#include <array>   // array
#include <cstddef> // size_t
#include <limits>  // numeric_limits

// The kernels the library measured and left out, kept here so that the comparison can be rerun.
namespace bench {

inline constexpr auto word_width = static_cast<unsigned>(std::numeric_limits<std::size_t>::digits);

// One step per halving of the distance a bit can still move: six for a 64-bit word.
inline constexpr auto suffix_steps = [] -> std::size_t {
        auto steps = 0UZ;
        for (auto width = word_width; width > 1U; width >>= 1U) {
                ++steps;
        }
        return steps;
}();

// The mask's ones lowest first, with an if on each source bit: the loop the rank kernel replaces.
[[nodiscard]] constexpr auto compress_by_walk_with_if(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        auto dst = 0UZ;
        for (auto next = 1UZ; mask != 0UZ; mask &= mask - 1UZ, next <<= 1U) {
                if ((src & mask & (0UZ - mask)) != 0UZ) {
                        dst |= next;
                }
        }
        return dst;
}

// The mask's ones lowest first, with an if on each source bit: the loop the masked walk replaces.
[[nodiscard]] constexpr auto expand_by_walk_with_if(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        auto dst = 0UZ;
        for (; mask != 0UZ; mask &= mask - 1UZ, src >>= 1U) {
                if ((src & 1UZ) != 0UZ) {
                        dst |= mask & (0UZ - mask);
                }
        }
        return dst;
}

// Bit i of the result is the parity of bits 0 through i: one shift and one xor per doubling.
[[nodiscard]] constexpr auto prefix_xor(std::size_t x) noexcept
        -> std::size_t
{
#pragma GCC unroll 8
        for (auto shift = 1U; shift < word_width; shift <<= 1U) {
                x ^= x << shift;
        }
        return x;
}

// Hacker's Delight, 2nd edition, section 7-4: each step moves the bits with an odd count of mask zeros below them.
[[nodiscard]] constexpr auto compress_by_parallel_suffix(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        src &= mask;
        auto zeros_below = ~mask << 1U;
#pragma GCC unroll 8
        for (auto shift = 1U; shift < word_width; shift <<= 1U) {
                auto const odd    = prefix_xor(zeros_below);
                auto const move   = odd & mask;
                mask              = (mask ^ move) | (move >> shift);
                auto const moving = src & move;
                src               = (src ^ moving) | (moving >> shift);
                zeros_below &= ~odd;
        }
        return src;
}

// Hacker's Delight, 2nd edition, section 7-5: the compress steps computed first, then undone in reverse.
[[nodiscard]] constexpr auto expand_by_parallel_suffix(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        auto const original = mask;
        auto moves          = std::array<std::size_t, suffix_steps>{};
        auto zeros_below    = ~mask << 1U;
#pragma GCC unroll 8
        for (auto step = 0UZ; step < suffix_steps; ++step) {
                auto const odd  = prefix_xor(zeros_below);
                auto const move = odd & mask;
                moves[step]     = move;
                mask            = (mask ^ move) | (move >> (1U << step));
                zeros_below &= ~odd;
        }
#pragma GCC unroll 8
        for (auto step = suffix_steps; step-- > 0UZ;) {
                auto const shifted = src << (1U << step);
                src                = (src & ~moves[step]) | (shifted & moves[step]);
        }
        return src & original;
}

} // namespace bench

#endif // XSTD_INTS_BENCH_ALTERNATIVES_HPP
