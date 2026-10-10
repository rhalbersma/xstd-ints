//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_BIT_REFERENCE_HPP
#define TEST_BIT_REFERENCE_HPP

#include <xstd/ints/cstdint.hpp> // XSTD_HAS_BIT_INT, bit_uint
#include <xstd/ints/limits.hpp>  // numeric_limits
#include <array>                 // array
#include <cstddef>               // size_t
#include <cstdint>               // uint64_t
#include <ranges>                // iota
#include <tuple>                 // tuple, tuple_cat
#include <type_traits>           // bool_constant
#include <utility>               // declval, integer_sequence

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

// The three references walk one bit at a time, sharing nothing with any implementation under test.
template<class T>
[[nodiscard]] constexpr auto reference_countl_zero(T x) noexcept
        -> int
{
        auto const ones = xstd::numeric_limits<T>::max();
        auto count      = 0;
        for (auto probe = static_cast<T>(ones ^ (ones >> 1U)); probe != T{0} and (x & probe) == T{0}; probe >>= 1U) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto reference_countr_zero(T x) noexcept
        -> int
{
        auto count = 0;
        for (auto probe = T{1}; probe != T{0} and (x & probe) == T{0}; probe += probe) {
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
                count += (x & T{1}) != T{0} ? 1 : 0;
        }
        return count;
}

// The four permutation references walk one bit at a time, as P3104R5's own illustrations do.
template<class T>
[[nodiscard]] constexpr auto reference_bit_reverse(T x) noexcept
        -> T
{
        auto result = T{0};
        for ([[maybe_unused]] auto const n : std::views::iota(0, xstd::numeric_limits<T>::digits)) {
                result = static_cast<T>(static_cast<T>(result << 1) | static_cast<T>(x & T{1}));
                x      = static_cast<T>(x >> 1);
        }
        return result;
}

template<class T>
[[nodiscard]] constexpr auto reference_bit_repeat(T x, int l) noexcept
        -> T
{
        auto result = T{0};
        for (auto const n : std::views::iota(0, xstd::numeric_limits<T>::digits)) {
                if (static_cast<T>(static_cast<T>(x >> (n % l)) & T{1}) != T{0}) {
                        result = static_cast<T>(result | static_cast<T>(T{1} << n));
                }
        }
        return result;
}

template<class T>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] constexpr auto reference_bit_compress(T x, T m) noexcept
        -> T
{
        auto result = T{0};
        auto j      = 0;
        for (auto const n : std::views::iota(0, xstd::numeric_limits<T>::digits)) {
                if (static_cast<T>(static_cast<T>(m >> n) & T{1}) != T{0}) {
                        result = static_cast<T>(result | static_cast<T>(static_cast<T>(static_cast<T>(x >> n) & T{1}) << j));
                        ++j;
                }
        }
        return result;
}

template<class T>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] constexpr auto reference_bit_expand(T x, T m) noexcept
        -> T
{
        auto result = T{0};
        auto j      = 0;
        for (auto const n : std::views::iota(0, xstd::numeric_limits<T>::digits)) {
                if (static_cast<T>(static_cast<T>(m >> n) & T{1}) != T{0}) {
                        result = static_cast<T>(result | static_cast<T>(static_cast<T>(static_cast<T>(x >> j) & T{1}) << n));
                        ++j;
                }
        }
        return result;
}

// Zero, one, all ones, and at every position the single bit, its complement, and the masks below and from it.
template<class T, class F>
constexpr auto for_each_edge_value(F f)
        -> void
{
        auto const ones = xstd::numeric_limits<T>::max();
        f(T{0});
        f(T{1});
        f(ones);

        // Doubling walks the bit up the width and wraps it to zero past the top: no shift by a variable count.
        auto below = T{0};
        for (auto bit = T{1}; bit != T{0}; bit += bit) {
                f(bit);
                f(static_cast<T>(ones ^ bit));
                f(below);
                f(static_cast<T>(ones ^ below));
                below |= bit;
        }
}

// The edge values at one position: its single bit, the mask below it, and the complements of both.
template<class T, class F>
constexpr auto for_each_edge_value_at(int k, F f)
        -> void
{
        auto const ones  = xstd::numeric_limits<T>::max();
        auto const bit   = static_cast<T>(T{1} << k);
        auto const below = static_cast<T>(bit - T{1});
        f(bit);
        f(static_cast<T>(ones ^ bit));
        f(below);
        f(static_cast<T>(ones ^ below));
}

// One constant evaluation per position, each a template argument, so that none runs into the evaluator's step limit.
template<auto holds_at, int... K>
[[nodiscard]] constexpr auto holds_at_every_position(std::integer_sequence<int, K...>) noexcept
        -> bool
{
        return (std::bool_constant<holds_at(K)>::value and ...);
}

// SplitMix64, so a run reproduces.
[[nodiscard]] constexpr auto next_split_mix_64(std::uint64_t& state) noexcept
        -> std::uint64_t
{
        state += 0x9e37'79b9'7f4a'7c15U;
        auto z = state;
        z      = (z ^ (z >> 30U)) * 0xbf58'476d'1ce4'e5b9U;
        z      = (z ^ (z >> 27U)) * 0x94d0'49bb'1331'11ebU;
        return z ^ (z >> 31U);
}

// 64 bits at a time until the width is filled, each earlier draw moved up by a constant 64 to make room.
template<class T>
[[nodiscard]] constexpr auto next_pseudo_random(std::uint64_t& state) noexcept
        -> T
{
        if constexpr (width<T> <= 64UZ) {
                return static_cast<T>(next_split_mix_64(state));
        } else {
                auto value = static_cast<T>(next_split_mix_64(state));
                for (auto k = 64UZ; k < width<T>; k += 64UZ) {
                        value <<= 64U;
                        value |= static_cast<T>(next_split_mix_64(state));
                }
                return value;
        }
}

// Every value up to 16 bits, and past that the edge values with a reproducible pseudo-random sample.
template<class T, class F>
constexpr auto for_each_sweep_value(F f)
        -> void
{
        if constexpr (width<T> <= 16UZ) {
                // Counting up wraps to zero after the maximum, which ends the walk over every value.
                auto value = T{0};
                do {
                        f(value);
                        value += T{1};
                } while (value != T{0});
        } else {
                for_each_edge_value<T>(f);
                auto state = std::uint64_t{width<T>};
                for ([[maybe_unused]] auto const sample : std::views::iota(0UZ, 256UZ)) {
                        f(next_pseudo_random<T>(state));
                }
        }
}

// Ones at every even position, a mask whose selected bits never neighbour each other.
template<class T>
[[nodiscard]] constexpr auto alternating_ones() noexcept
        -> T
{
        auto result = T{0};
        // Doubling twice walks the bit up two places and to zero past the top; Abseil's fallback * is not constexpr.
        for (auto bit = T{1}; bit != T{0}; bit = static_cast<T>((bit + bit) + (bit + bit))) {
                result = static_cast<T>(result | bit);
        }
        return result;
}

// Every pair up to 8 bits; past that every swept value against three fixed partners on either side, and random pairs.
template<class T, class F>
constexpr auto for_each_sweep_pair(F f)
        -> void
{
        if constexpr (width<T> <= 8UZ) {
                for_each_sweep_value<T>([&](T m) -> void { for_each_sweep_value<T>([&](T x) -> void { f(x, m); }); });
        } else {
                auto state          = std::uint64_t{width<T>};
                auto const noise    = next_pseudo_random<T>(state);
                auto const partners = std::array{xstd::numeric_limits<T>::max(), alternating_ones<T>(), noise};
                for_each_sweep_value<T>([&](T value) -> void {
                        for (auto const partner : partners) {
                                f(value, partner);
                                f(partner, value);
                        }
                });
                for ([[maybe_unused]] auto const sample : std::views::iota(0UZ, 1024UZ)) {
                        auto const x = next_pseudo_random<T>(state);
                        f(x, next_pseudo_random<T>(state));
                }
        }
}

} // namespace test

#endif // TEST_BIT_REFERENCE_HPP
