//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>        // int128, XSTD_HAS_INT128
#include <xstd/ints/cstdlib/abs.hpp>           // abs
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <cstdint>                             // int8_t, int32_t, int64_t, uint8_t
#include <iostream>                            // cout
#include <type_traits>                         // is_same_v

// As with std::abs, a type narrower than int is promoted, so the minimum of int8_t has a magnitude.
static_assert(std::is_same_v<decltype(xstd::abs(std::int8_t{-128})), int>);
static_assert(xstd::abs(std::int8_t{-128}) == 128);
static_assert(std::is_same_v<decltype(xstd::abs(std::uint8_t{255})), int>);

// From int up, the type is kept.
static_assert(std::is_same_v<decltype(xstd::abs(std::int64_t{-1})), std::int64_t>);

#ifdef XSTD_HAS_INT128
// A 128-bit integer is taken as well, which std::abs is not required to take.
constexpr auto int128_max = xstd::numeric_limits<xstd::int128>::max();
static_assert(std::is_same_v<decltype(xstd::abs(int128_max)), xstd::int128>);
static_assert(xstd::abs(-int128_max) == int128_max);
#endif

auto main() -> int
{
        constexpr auto int32_max = xstd::numeric_limits<std::int32_t>::max();
        std::cout << "abs(-7)           = " << xstd::abs(std::int32_t{-7}) << '\n';
        std::cout << "abs(0)            = " << xstd::abs(std::int32_t{0}) << '\n';
        std::cout << "abs(-int32_max)   = " << xstd::abs(-int32_max) << '\n';
        std::cout << "abs(int8_t{-128}) = " << xstd::abs(std::int8_t{-128}) << '\n';
}
