//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>        // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/cstdlib/unsigned_abs.hpp>  // unsigned_abs
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <cstdint>                             // int8_t, int32_t, uint8_t, uint32_t
#include <iostream>                            // cout
#include <type_traits>                         // is_same_v

// The result is the unsigned counterpart of a signed type, and the type itself where it is unsigned.
static_assert(std::is_same_v<decltype(xstd::unsigned_abs(std::int8_t{-1})), std::uint8_t>);
static_assert(std::is_same_v<decltype(xstd::unsigned_abs(std::uint32_t{1})), std::uint32_t>);

#ifdef XSTD_HAS_INT128
// The magnitude of the minimum is one more than the maximum, which only the unsigned counterpart holds.
constexpr auto int128_max = xstd::numeric_limits<xstd::int128>::max();
constexpr auto int128_min = xstd::numeric_limits<xstd::int128>::min();
static_assert(xstd::unsigned_abs(int128_min) == static_cast<xstd::uint128>(int128_max) + xstd::uint128{1});
#endif

auto main() -> int
{
        // Total over the minimum, whose negation int32_t cannot hold and where abs has a precondition.
        constexpr auto int32_min = xstd::numeric_limits<std::int32_t>::min();
        std::cout << "unsigned_abs(int32_min)    = " << xstd::unsigned_abs(int32_min) << '\n';
        std::cout << "unsigned_abs(-7)           = " << xstd::unsigned_abs(std::int32_t{-7}) << '\n';
        std::cout << "unsigned_abs(int8_t{-128}) = " << static_cast<unsigned>(xstd::unsigned_abs(std::int8_t{-128})) << '\n';
}
