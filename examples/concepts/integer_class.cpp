//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer_class.hpp> // integer_class
#include <xstd/ints/cstdint/int128.hpp>         // int128, uint128, XSTD_HAS_INT128
#include <cstddef>                              // byte
#include <cstdint>                              // int8_t, int64_t, uint64_t
#include <iostream>                             // cout
#include <limits>                               // numeric_limits

static_assert(xstd::integer_class<std::int8_t>);
static_assert(xstd::integer_class<std::uint64_t const>);
static_assert(xstd::integer_class<char16_t>);

// bool has no ++ or --, and byte has no arithmetic.
static_assert(not xstd::integer_class<bool>);
static_assert(not xstd::integer_class<std::byte>);

// The number of decimal digits of value, its sign aside, for any integer, built-in or a class.
template<xstd::integer_class T>
[[nodiscard]] constexpr auto decimal_digits(T value)
        -> int
{
        auto const ten  = static_cast<T>(10);
        auto const zero = static_cast<T>(0);
        auto digits     = 1;
        while ((value = static_cast<T>(value / ten)) != zero) {
                ++digits;
        }
        return digits;
}

#ifdef XSTD_HAS_INT128
static_assert(xstd::integer_class<xstd::int128>);
static_assert(decimal_digits(static_cast<xstd::uint128>(-1)) == 39);
#endif

auto main() -> int
{
        std::cout << decimal_digits(std::int8_t{-128}) << '\n';
        std::cout << decimal_digits(std::numeric_limits<std::int64_t>::min()) << '\n';
        std::cout << decimal_digits(std::numeric_limits<std::uint64_t>::max()) << '\n';
        std::cout << decimal_digits(char16_t{0xFFFF}) << '\n';
}
