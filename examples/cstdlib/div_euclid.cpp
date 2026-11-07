//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>     // int128, XSTD_HAS_INT128
#include <xstd/ints/cstdlib/div.hpp>        // div
#include <xstd/ints/cstdlib/div_euclid.hpp> // div_euclid, div_result
#include <cstdint>                          // int32_t, uint32_t
#include <initializer_list>                 // initializer_list
#include <iomanip>                          // setw
#include <iostream>                         // cout

// Over an unsigned type it is truncated division.
static_assert(xstd::div_euclid(std::uint32_t{7}, std::uint32_t{2}) == xstd::div(std::uint32_t{7}, std::uint32_t{2}));

#ifdef XSTD_HAS_INT128
static_assert(xstd::div_euclid(xstd::int128{-7}, xstd::int128{-2}) == xstd::div_result<xstd::int128>{.quotient = 4, .remainder = 1});
#endif

namespace {

auto show(std::int32_t numer, std::int32_t denom)
        -> void
{
        auto const [quotient, remainder] = xstd::div_euclid(numer, denom);
        std::cout << std::setw(2) << numer << " / " << std::setw(2) << denom << " = " << std::setw(2) << quotient << " remainder " << std::setw(2) << remainder << '\n';
}

} // namespace

auto main() -> int
{
        // The remainder is never negative, whatever the signs of the operands.
        show(7, 2);
        show(-7, 2);
        show(7, -2);
        show(-7, -2);

        // A step backward from the front of a ring of five slots lands on the last one.
        constexpr auto slots = std::int32_t{5};
        for (auto const position : {std::int32_t{-6}, std::int32_t{-1}, std::int32_t{0}, std::int32_t{7}}) {
                std::cout << "position " << std::setw(2) << position << " is slot " << xstd::div_euclid(position, slots).remainder << '\n';
        }
}
