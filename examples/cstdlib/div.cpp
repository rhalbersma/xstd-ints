//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp> // int128, XSTD_HAS_INT128
#include <xstd/ints/cstdlib/div.hpp>    // div, div_result
#include <cstdint>                      // int8_t, int32_t
#include <iomanip>                      // setw
#include <iostream>                     // cout
#include <type_traits>                  // is_same_v

// The result keeps the operands' own width, where std::div would promote an int8_t to int.
static_assert(std::is_same_v<decltype(xstd::div(std::int8_t{-7}, std::int8_t{2})), xstd::div_result<std::int8_t>>);

#ifdef XSTD_HAS_INT128
static_assert(xstd::div(xstd::int128{-7}, xstd::int128{2}) == xstd::div_result<xstd::int128>{.quotient = -3, .remainder = -1});
#endif

namespace {

auto show(std::int32_t numer, std::int32_t denom)
        -> void
{
        auto const [quotient, remainder] = xstd::div(numer, denom);
        std::cout << std::setw(2) << numer << " / " << std::setw(2) << denom << " = " << std::setw(2) << quotient << " remainder " << std::setw(2) << remainder << '\n';
}

} // namespace

auto main() -> int
{
        // The quotient rounds toward zero, and a nonzero remainder has the sign of the numerator.
        show(7, 2);
        show(-7, 2);
        show(7, -2);
        show(-7, -2);
}
