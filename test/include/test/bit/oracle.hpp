//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_BIT_ORACLE_HPP
#define TEST_BIT_ORACLE_HPP

#include <xstd/ints/limits.hpp>                    // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp>     // is_signed_v
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <test/bit/shift_count.hpp>                // shift_count_t
#include <ranges>                                  // iota

namespace test {

// The counting oracles walk one bit at a time, sharing nothing with any implementation under test.
template<class T>
[[nodiscard]] constexpr auto oracle_countl_zero(T x) noexcept
        -> int
{
        auto const ones = xstd::numeric_limits<T>::max();
        auto count      = 0;
        for (auto probe = static_cast<T>(ones ^ static_cast<T>(ones >> 1U)); probe != T{0} and (x & probe) == T{0}; probe = static_cast<T>(probe >> 1U)) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto oracle_countr_zero(T x) noexcept
        -> int
{
        auto count = 0;
        for (auto probe = T{1}; probe != T{0} and (x & probe) == T{0}; probe = static_cast<T>(probe + probe)) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto oracle_popcount(T x) noexcept
        -> int
{
        auto count = 0;
        for (; x != T{0}; x = static_cast<T>(x >> 1U)) {
                count += (x & T{1}) != T{0} ? 1 : 0;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto oracle_countl_one(T x) noexcept
        -> int
{
        auto const ones = xstd::numeric_limits<T>::max();
        auto count      = 0;
        for (auto probe = static_cast<T>(ones ^ static_cast<T>(ones >> 1U)); probe != T{0} and (x & probe) != T{0}; probe = static_cast<T>(probe >> 1U)) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto oracle_countr_one(T x) noexcept
        -> int
{
        auto count = 0;
        for (auto probe = T{1}; probe != T{0} and (x & probe) != T{0}; probe = static_cast<T>(probe + probe)) {
                ++count;
        }
        return count;
}

template<class T>
[[nodiscard]] constexpr auto oracle_bit_width(T x) noexcept
        -> int
{
        auto width = 0;
        for (; x != T{0}; x = static_cast<T>(x >> 1U)) {
                ++width;
        }
        return width;
}

template<class T>
[[nodiscard]] constexpr auto oracle_has_single_bit(T x) noexcept
        -> bool
{
        return oracle_popcount(x) == 1;
}

// Clearing the lowest one until a single one is left.
template<class T>
[[nodiscard]] constexpr auto oracle_bit_floor(T x) noexcept
        -> T
{
        while (static_cast<T>(x & static_cast<T>(x - T{1})) != T{0}) {
                x = static_cast<T>(x & static_cast<T>(x - T{1}));
        }
        return x;
}

// Doubling from one until x is reached; past the top bit the doubling wraps to zero, which ends it.
template<class T>
[[nodiscard]] constexpr auto oracle_bit_ceil(T x) noexcept
        -> T
{
        auto power = T{1};
        while (power != T{0} and power < x) {
                power = static_cast<T>(power + power);
        }
        return power;
}

// One place at a time, the bit leaving the top coming back in at the bottom, as many times as the count says.
template<class T>
[[nodiscard]] constexpr auto oracle_rotl(T x, int s) noexcept
        -> T
{
        auto const ones = xstd::numeric_limits<T>::max();
        auto const top  = static_cast<T>(ones ^ static_cast<T>(ones >> 1U));
        auto const N    = xstd::numeric_limits<T>::digits;
        for ([[maybe_unused]] auto const n : std::views::iota(0, ((s % N) + N) % N)) {
                x = static_cast<T>(static_cast<T>(x << 1U) | ((x & top) != T{0} ? T{1} : T{0}));
        }
        return x;
}

// One byte at a time, through the unsigned counterpart: byte n from the bottom becomes byte n from the top.
template<class T>
[[nodiscard]] constexpr auto oracle_byteswap(T x) noexcept
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

// P3793's x times two to the count, one place at a time: doubling in the unsigned counterpart, halving by division.
template<class T>
[[nodiscard]] constexpr auto oracle_shift(T x, long long count) noexcept
        -> T
{
        using U          = xstd::make_unsigned_t<T>;
        auto const W     = static_cast<long long>(xstd::numeric_limits<U>::digits);
        auto const steps = count < 0 ? (-count < W ? -count : W) : (count < W ? count : W);
        for ([[maybe_unused]] auto const n : std::views::iota(0LL, steps)) {
                if (count >= 0) {
                        x = static_cast<T>(static_cast<U>(static_cast<U>(x) << 1U));
                } else if constexpr (xstd::is_signed_v<T>) {
                        // Division truncates, so an odd negative value takes one more step down to its floor.
                        x = static_cast<T>(static_cast<T>(x / T{2}) - (x < T{0} and x % T{2} != T{0} ? T{1} : T{0}));
                } else {
                        x = static_cast<T>(x / T{2});
                }
        }
        return x;
}

// The four permutation oracles walk one bit at a time, as P3104R5's own illustrations do.
template<class T>
[[nodiscard]] constexpr auto oracle_bit_reverse(T x) noexcept
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
[[nodiscard]] constexpr auto oracle_bit_repeat(T x, int l) noexcept
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
[[nodiscard]] constexpr auto oracle_bit_compress(T x, T m) noexcept
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
[[nodiscard]] constexpr auto oracle_bit_expand(T x, T m) noexcept
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

// The position of x's k-th one, counted off one bit at a time; the width where x has no k-th one.
template<class T>
[[nodiscard]] constexpr auto oracle_bit_select(T x, int k) noexcept
        -> int
{
        for (auto const n : std::views::iota(0, xstd::numeric_limits<T>::digits)) {
                if (static_cast<T>(x & T{1}) != T{0}) {
                        if (k == 0) {
                                return n;
                        }
                        --k;
                }
                x = static_cast<T>(x >> 1U);
        }
        return xstd::numeric_limits<T>::digits;
}

} // namespace test

#endif // TEST_BIT_ORACLE_HPP
