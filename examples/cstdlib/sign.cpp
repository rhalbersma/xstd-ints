//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>        // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/cstdlib/sign.hpp>          // sign
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <cstdint>                             // int8_t, int32_t, uint64_t
#include <initializer_list>                    // initializer_list
#include <iomanip>                             // setw
#include <iostream>                            // cout
#include <type_traits>                         // is_same_v

// The result is an int, whatever the argument's type.
static_assert(std::is_same_v<decltype(xstd::sign(std::int8_t{-5})), int>);
static_assert(std::is_same_v<decltype(xstd::sign(std::uint64_t{5})), int>);

// Over an unsigned type it is 0 or +1.
static_assert(xstd::sign(xstd::numeric_limits<std::uint64_t>::max()) == +1);

#ifdef XSTD_HAS_INT128
static_assert(xstd::sign(xstd::numeric_limits<xstd::int128>::min()) == -1);
static_assert(xstd::sign(xstd::uint128{0}) == 0);
#endif

auto main() -> int
{
        // It is total, the minimum included, whose magnitude int32_t cannot hold.
        constexpr auto lo = xstd::numeric_limits<std::int32_t>::min();
        constexpr auto hi = xstd::numeric_limits<std::int32_t>::max();
        for (auto const x : {lo, std::int32_t{-7}, std::int32_t{0}, std::int32_t{7}, hi}) {
                std::cout << "sign(" << std::setw(11) << x << ") = " << std::setw(2) << xstd::sign(x) << '\n';
        }
}
