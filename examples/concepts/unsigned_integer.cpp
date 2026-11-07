//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer.hpp>          // integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdint/int128.hpp>            // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <bit>                                     // popcount
#include <cstdint>                                 // int8_t, int32_t, int64_t, uint8_t, uint32_t, uint64_t
#include <iostream>                                // cout

static_assert(xstd::unsigned_integer<std::uint8_t>);
static_assert(xstd::unsigned_integer<std::uint64_t const>);
static_assert(not xstd::unsigned_integer<std::int32_t>);
static_assert(not xstd::unsigned_integer<char8_t>);

#ifdef XSTD_HAS_INT128
static_assert(xstd::unsigned_integer<xstd::uint128>);
static_assert(not xstd::unsigned_integer<xstd::int128>);
#endif

// The set bits of an unsigned integer, which std::popcount takes.
template<xstd::unsigned_integer T>
[[nodiscard]] constexpr auto ones(T value) noexcept
        -> int
{
        return std::popcount(value);
}

// The set bits of a signed integer's two's complement, through its unsigned counterpart.
template<xstd::integer T>
[[nodiscard]] constexpr auto ones(T value) noexcept
        -> int
{
        return ones(static_cast<xstd::make_unsigned_t<T>>(value));
}

auto main() -> int
{
        std::cout << ones(std::uint32_t{0xF0}) << '\n';
        std::cout << ones(std::int8_t{-1}) << '\n';
        std::cout << ones(std::int64_t{-2}) << '\n';
}
