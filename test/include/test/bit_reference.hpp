//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_BIT_REFERENCE_HPP
#define TEST_BIT_REFERENCE_HPP

#include <xstd/ints/cstdint.hpp> // XSTD_HAS_BIT_INT, bit_uint
#include <xstd/ints/limits.hpp>  // numeric_limits
#include <cstddef>               // size_t
#include <cstdint>               // uint64_t
#include <ranges>                // iota
#include <tuple>                 // tuple, tuple_cat
#include <utility>               // declval

// Every <xstd/ints/bit/> function answers unsigned _BitInt(N) where the build has the type and the three builtins.
#ifdef XSTD_HAS_BIT_INT
#if __has_builtin(__builtin_clzg) and __has_builtin(__builtin_ctzg) and __has_builtin(__builtin_popcountg)
#define TEST_HAS_BIT_PRECISE_BIT
#endif
#endif

namespace test {

// Widths on both sides of every word boundary, and odd ones that fill no byte, each as far as the build reaches.
#ifdef TEST_HAS_BIT_PRECISE_BIT
using bit_precise_narrow_sweep_types = std::tuple<xstd::bit_uint<2>, xstd::bit_uint<4>, xstd::bit_uint<5>, xstd::bit_uint<13>, xstd::bit_uint<64>>;

#if __BITINT_MAXWIDTH__ >= 127
using bit_precise_wide_sweep_types = std::tuple<xstd::bit_uint<65>, xstd::bit_uint<127>>;
#elif __BITINT_MAXWIDTH__ >= 65
using bit_precise_wide_sweep_types = std::tuple<xstd::bit_uint<65>>;
#else
using bit_precise_wide_sweep_types = std::tuple<>;
#endif

#if __BITINT_MAXWIDTH__ >= 147
using bit_precise_widest_sweep_types = std::tuple<xstd::bit_uint<129>, xstd::bit_uint<147>>;
#elif __BITINT_MAXWIDTH__ >= 129
using bit_precise_widest_sweep_types = std::tuple<xstd::bit_uint<129>>;
#else
using bit_precise_widest_sweep_types = std::tuple<>;
#endif

using bit_precise_sweep_types = decltype(std::tuple_cat(
        std::declval<bit_precise_narrow_sweep_types>(), std::declval<bit_precise_wide_sweep_types>(),
        std::declval<bit_precise_widest_sweep_types>()
));
#else
using bit_precise_sweep_types = std::tuple<>;
#endif

template<class T>
inline constexpr auto width = static_cast<std::size_t>(xstd::numeric_limits<T>::digits);

template<class T>
[[nodiscard]] constexpr auto bit_at(T x, std::size_t i) noexcept
        -> bool
{
        return static_cast<T>(static_cast<T>(x >> i) & T{1}) != T{0};
}

// The three references read one bit at a time, sharing nothing with any implementation under test.
template<class T>
[[nodiscard]] constexpr auto reference_countl_zero(T x) noexcept
        -> int
{
        constexpr auto N = width<T>;
        auto count       = 0;
        for (auto i = N - 1UZ; i < N and not bit_at(x, i); --i) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto reference_countr_zero(T x) noexcept
        -> int
{
        constexpr auto N = width<T>;
        auto count       = 0;
        for (auto i = 0UZ; i < N and not bit_at(x, i); ++i) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto reference_popcount(T x) noexcept
        -> int
{
        auto count = 0;
        for (; x != T{0}; x >>= 1U) {
                count += bit_at(x, 0UZ) ? 1 : 0;
        }
        return count;
}

// Zero, one, all ones, and at every position the single bit, its complement, and the masks below and from it.
template<class T, class F>
constexpr auto for_each_edge_value(F f)
        -> void
{
        constexpr auto ones = static_cast<T>(~T{0});
        f(T{0});
        f(T{1});
        f(ones);
        for (auto const i : std::views::iota(0UZ, width<T>)) {
                auto const bit = static_cast<T>(T{1} << i);
                f(bit);
                f(static_cast<T>(~bit));
                f(static_cast<T>(bit - T{1}));
                f(static_cast<T>(ones << i));
        }
}

// SplitMix64, so a run reproduces: 64 bits at a time until the width is filled.
template<class T>
[[nodiscard]] constexpr auto next_pseudo_random(std::uint64_t& state) noexcept
        -> T
{
        auto value = T{0};
        for (auto k = 0UZ; k < width<T>; k += 64UZ) {
                state += 0x9e37'79b9'7f4a'7c15U;
                auto z = state;
                z      = (z ^ (z >> 30U)) * 0xbf58'476d'1ce4'e5b9U;
                z      = (z ^ (z >> 27U)) * 0x94d0'49bb'1331'11ebU;
                z ^= z >> 31U;
                value = static_cast<T>(value | static_cast<T>(static_cast<T>(z) << k));
        }
        return value;
}

// Every value up to 13 bits, and past that the edge values with a reproducible pseudo-random sample.
template<class T, class F>
constexpr auto for_each_sweep_value(F f)
        -> void
{
        if constexpr (width<T> <= 13UZ) {
                for (auto const v : std::views::iota(0UZ, 1UZ << width<T>)) {
                        f(static_cast<T>(v));
                }
        } else {
                for_each_edge_value<T>(f);
                auto state = std::uint64_t{width<T>};
                for ([[maybe_unused]] auto const sample : std::views::iota(0UZ, 256UZ)) {
                        f(next_pseudo_random<T>(state));
                }
        }
}

} // namespace test

#endif // TEST_BIT_REFERENCE_HPP
