//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>    // int128, XSTD_HAS_INT128
#include <xstd/ints/cstdlib/div.hpp>       // div
#include <xstd/ints/cstdlib/div_floor.hpp> // div_floor, div_result
#include <cstdint>                         // int32_t, int64_t
#include <iomanip>                         // setw
#include <iostream>                        // cout

// Wherever the division is exact, it agrees with truncated division.
static_assert(xstd::div_floor(-8, 2) == xstd::div(-8, 2));

#ifdef XSTD_HAS_INT128
static_assert(xstd::div_floor(xstd::int128{-7}, xstd::int128{2}) == xstd::div_result<xstd::int128>{.quotient = -4, .remainder = 1});
#endif

namespace {

auto show(std::int32_t numer, std::int32_t denom)
        -> void
{
        auto const [quotient, remainder] = xstd::div_floor(numer, denom);
        std::cout << std::setw(2) << numer << " / " << std::setw(2) << denom << " = " << std::setw(2) << quotient << " remainder " << std::setw(2) << remainder << '\n';
}

} // namespace

auto main() -> int
{
        // The quotient rounds toward negative infinity, and a nonzero remainder has the sign of the denominator.
        show(7, 2);
        show(-7, 2);
        show(7, -2);
        show(-7, -2);

        // A timestamp before the epoch still splits into a minute and the seconds into it.
        auto const [minute, second] = xstd::div_floor(std::int64_t{-90}, std::int64_t{60});
        std::cout << "-90 s is minute " << minute << ", second " << second << '\n';
}
