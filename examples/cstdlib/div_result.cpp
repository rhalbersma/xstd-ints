//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>     // int128, XSTD_HAS_INT128
#include <xstd/ints/cstdlib/div_result.hpp> // div_result
#include <cstdint>                          // int32_t, uint8_t
#include <iostream>                         // cout
#include <type_traits>                      // is_aggregate_v, is_same_v

// An aggregate of two members of one type, quotient then remainder.
static_assert(std::is_aggregate_v<xstd::div_result<std::int32_t>>);
constexpr auto d = xstd::div_result<std::int32_t>{.quotient = 3, .remainder = 1};
static_assert(d.quotient == 3 and d.remainder == 1);

// Two arguments of one type deduce it, and equality compares memberwise.
static_assert(std::is_same_v<decltype(xstd::div_result{std::uint8_t{3}, std::uint8_t{1}}), xstd::div_result<std::uint8_t>>);
static_assert(xstd::div_result{3, 1} == d);
static_assert(xstd::div_result{3, 2} != d);

#ifdef XSTD_HAS_INT128
// One template serves every integer, where the standard has div_t, ldiv_t, lldiv_t and imaxdiv_t.
static_assert(xstd::div_result<xstd::int128>{.quotient = 3, .remainder = 1}.remainder == 1);
#endif

auto main() -> int
{
        // A structured binding names the members in order.
        auto const [quotient, remainder] = xstd::div_result{std::int32_t{-3}, std::int32_t{-1}};
        std::cout << "quotient " << quotient << ", remainder " << remainder << '\n';

        auto const u = xstd::div_result{std::uint8_t{2}, std::uint8_t{1}};
        std::cout << "quotient " << static_cast<unsigned>(u.quotient) << ", remainder " << static_cast<unsigned>(u.remainder) << '\n';
}
