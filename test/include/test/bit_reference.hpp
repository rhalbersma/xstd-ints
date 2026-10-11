//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_BIT_REFERENCE_HPP
#define TEST_BIT_REFERENCE_HPP

#include <xstd/ints/cstdint.hpp>                   // XSTD_HAS_BIT_INT, bit_uint
#include <xstd/ints/limits.hpp>                    // numeric_limits
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <array>                                   // array
#include <cstddef>                                 // size_t
#include <cstdint>                                 // uint64_t
#include <ranges>                                  // iota
#include <tuple>                                   // tuple, tuple_cat
#include <type_traits>                             // bool_constant, conditional_t, is_class_v
#include <utility>                                 // declval, integer_sequence

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

// Unsigned for a builtin shift, so no count is signed; int for a class type's, as absl::uint128's operator<< takes.
template<class T>
using shift_count_t = std::conditional_t<std::is_class_v<T>, int, unsigned>;

// The counting references walk one bit at a time, sharing nothing with any implementation under test.
template<class T>
[[nodiscard]] constexpr auto reference_countl_zero(T x) noexcept
        -> int
{
        auto const ones = xstd::numeric_limits<T>::max();
        auto count      = 0;
        for (auto probe = static_cast<T>(ones ^ (ones >> 1U)); probe != T{0} and (x & probe) == T{0}; probe = static_cast<T>(probe >> 1U)) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto reference_countr_zero(T x) noexcept
        -> int
{
        auto count = 0;
        for (auto probe = T{1}; probe != T{0} and (x & probe) == T{0}; probe = static_cast<T>(probe + probe)) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto reference_popcount(T x) noexcept
        -> int
{
        auto count = 0;
        for (; x != T{0}; x = static_cast<T>(x >> 1U)) {
                count += (x & T{1}) != T{0} ? 1 : 0;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto reference_countl_one(T x) noexcept
        -> int
{
        auto const ones = xstd::numeric_limits<T>::max();
        auto count      = 0;
        for (auto probe = static_cast<T>(ones ^ (ones >> 1U)); probe != T{0} and (x & probe) != T{0}; probe = static_cast<T>(probe >> 1U)) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto reference_countr_one(T x) noexcept
        -> int
{
        auto count = 0;
        for (auto probe = T{1}; probe != T{0} and (x & probe) != T{0}; probe = static_cast<T>(probe + probe)) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto reference_bit_width(T x) noexcept
        -> int
{
        auto width = 0;
        for (; x != T{0}; x = static_cast<T>(x >> 1U)) {
                ++width;
        }
        return width;
}

template<class T>
[[nodiscard]] constexpr auto reference_has_single_bit(T x) noexcept
        -> bool
{
        return reference_popcount(x) == 1;
}

// Clearing the lowest one until a single one is left.
template<class T>
[[nodiscard]] constexpr auto reference_bit_floor(T x) noexcept
        -> T
{
        while (static_cast<T>(x & static_cast<T>(x - T{1})) != T{0}) {
                x = static_cast<T>(x & static_cast<T>(x - T{1}));
        }
        return x;
}

// Doubling from one until x is reached, for an x at most the top bit.
template<class T>
[[nodiscard]] constexpr auto reference_bit_ceil(T x) noexcept
        -> T
{
        auto power = T{1};
        while (power < x) {
                power = static_cast<T>(power + power);
        }
        return power;
}

// One place at a time, the bit leaving the top coming back in at the bottom, as many times as the count says.
template<class T>
[[nodiscard]] constexpr auto reference_rotl(T x, int s) noexcept
        -> T
{
        auto const ones = xstd::numeric_limits<T>::max();
        auto const top  = static_cast<T>(ones ^ (ones >> 1U));
        auto const N    = xstd::numeric_limits<T>::digits;
        for ([[maybe_unused]] auto const n : std::views::iota(0, ((s % N) + N) % N)) {
                x = static_cast<T>(static_cast<T>(x << 1U) | ((x & top) != T{0} ? T{1} : T{0}));
        }
        return x;
}

// One byte at a time, through the unsigned counterpart: byte n from the bottom becomes byte n from the top.
template<class T>
[[nodiscard]] constexpr auto reference_byteswap(T x) noexcept
        -> T
{
        using U          = xstd::make_unsigned_t<T>;
        using shift      = shift_count_t<U>;
        auto const bytes = xstd::numeric_limits<U>::digits / 8;
        auto result      = U{0};
        for (auto const n : std::views::iota(0, bytes)) {
                // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const& byte = static_cast<U>(static_cast<U>(static_cast<U>(x) >> static_cast<shift>(8 * n)) & U{0xFF});
                result           = static_cast<U>(result | static_cast<U>(byte << static_cast<shift>(8 * (bytes - 1 - n))));
        }
        return static_cast<T>(result);
}

// P3793's x times two to the count, one place at a time: doubling in the unsigned counterpart, halving in T.
template<class T>
[[nodiscard]] constexpr auto reference_shift(T x, long long count) noexcept
        -> T
{
        using U          = xstd::make_unsigned_t<T>;
        auto const W     = static_cast<long long>(xstd::numeric_limits<U>::digits);
        auto const steps = count < 0 ? (-count < W ? -count : W) : (count < W ? count : W);
        for ([[maybe_unused]] auto const n : std::views::iota(0LL, steps)) {
                x = count < 0 ? static_cast<T>(x >> 1U) : static_cast<T>(static_cast<U>(static_cast<U>(x) << 1U));
        }
        return x;
}

// The four permutation references walk one bit at a time, as P3104R5's own illustrations do.
template<class T>
[[nodiscard]] constexpr auto reference_bit_reverse(T x) noexcept
        -> T
{
        auto result = T{0};
        for ([[maybe_unused]] auto const n : std::views::iota(0, xstd::numeric_limits<T>::digits)) {
                result = static_cast<T>(static_cast<T>(result << 1U) | static_cast<T>(x & T{1}));
                x      = static_cast<T>(x >> 1U);
        }
        return result;
}

template<class T>
[[nodiscard]] constexpr auto reference_bit_repeat(T x, int l) noexcept
        -> T
{
        using shift_count = shift_count_t<T>;
        auto const period = static_cast<shift_count>(l);
        auto result       = T{0};
        for (auto const n : std::views::iota(shift_count{0}, shift_count{xstd::numeric_limits<T>::digits})) {
                if (static_cast<T>(static_cast<T>(x >> (n % period)) & T{1}) != T{0}) {
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
        using shift_count = shift_count_t<T>;
        auto result       = T{0};
        auto j            = shift_count{0};
        for (auto const n : std::views::iota(shift_count{0}, shift_count{xstd::numeric_limits<T>::digits})) {
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
        using shift_count = shift_count_t<T>;
        auto result       = T{0};
        auto j            = shift_count{0};
        for (auto const n : std::views::iota(shift_count{0}, shift_count{xstd::numeric_limits<T>::digits})) {
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
        for (auto bit = T{1}; bit != T{0}; bit = static_cast<T>(bit + bit)) {
                f(bit);
                f(static_cast<T>(ones ^ bit));
                f(below);
                f(static_cast<T>(ones ^ below));
                below = static_cast<T>(below | bit);
        }
}

// The edge values at one position: its single bit, the mask below it, and the complements of both.
template<class T, class F>
constexpr auto for_each_edge_value_at(int k, F f)
        -> void
{
        auto const ones  = xstd::numeric_limits<T>::max();
        auto const bit   = static_cast<T>(T{1} << static_cast<shift_count_t<T>>(k));
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
                        value = static_cast<T>(value + T{1});
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
                auto const partners = std::array{xstd::numeric_limits<T>::max(), alternating_ones<T>(), next_pseudo_random<T>(state)};
                for_each_sweep_value<T>([&](T value) -> void {
                        for (auto const& partner : partners) {
                                f(value, partner);
                                f(partner, value);
                        }
                });
                for ([[maybe_unused]] auto const sample : std::views::iota(0UZ, 1024UZ)) {
                        // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                        auto const& x = next_pseudo_random<T>(state);
                        f(x, next_pseudo_random<T>(state));
                }
        }
}

} // namespace test

#endif // TEST_BIT_REFERENCE_HPP
