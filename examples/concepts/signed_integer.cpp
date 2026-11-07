//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer.hpp>        // integer
#include <xstd/ints/concepts/signed_integer.hpp> // signed_integer
#include <xstd/ints/cstdint/int128.hpp>          // int128, uint128, XSTD_HAS_INT128
#include <cstdint>                               // int8_t, int32_t, int64_t, uint8_t, uint32_t
#include <iostream>                              // boolalpha, cout

static_assert(xstd::signed_integer<std::int8_t>);
static_assert(xstd::signed_integer<std::int32_t const>);
static_assert(not xstd::signed_integer<std::uint32_t>);
static_assert(not xstd::signed_integer<char>);

#ifdef XSTD_HAS_INT128
static_assert(xstd::signed_integer<xstd::int128>);
static_assert(not xstd::signed_integer<xstd::uint128>);
#endif

// An unsigned value is never negative, and comparing it with zero is what compilers warn about.
template<xstd::integer T>
[[nodiscard]] constexpr auto is_negative(T) noexcept
        -> bool
{
        return false;
}

// Chosen over the integer overload for a signed argument, signed_integer subsuming integer.
template<xstd::signed_integer T>
[[nodiscard]] constexpr auto is_negative(T value) noexcept
        -> bool
{
        return value < static_cast<T>(0);
}

auto main() -> int
{
        std::cout << std::boolalpha;
        std::cout << is_negative(std::int8_t{-1}) << '\n';
        std::cout << is_negative(std::int64_t{42}) << '\n';
        std::cout << is_negative(std::uint8_t{255}) << '\n';
}
