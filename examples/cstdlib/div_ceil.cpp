//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>        // int128, XSTD_HAS_INT128
#include <xstd/ints/cstdlib/div_ceil.hpp>      // div_ceil, div_result
#include <xstd/ints/cstdlib/div_floor.hpp>     // div_floor
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <cstdint>                             // int32_t, uint32_t
#include <initializer_list>                    // initializer_list
#include <iomanip>                             // setw
#include <iostream>                            // cout

// The quotient is one more than the floored one wherever that leaves a remainder.
static_assert(xstd::div_ceil(-7, 2).quotient == xstd::div_floor(-7, 2).quotient + 1);
static_assert(xstd::div_ceil(-8, 2).quotient == xstd::div_floor(-8, 2).quotient);

// Unsigned, an inexact quotient still rounds up, and the negative remainder wraps modulo 2^32.
static_assert(xstd::div_ceil(std::uint32_t{7}, std::uint32_t{2}) == xstd::div_result<std::uint32_t>{.quotient = 4, .remainder = xstd::numeric_limits<std::uint32_t>::max()});

#ifdef XSTD_HAS_INT128
static_assert(xstd::div_ceil(xstd::int128{7}, xstd::int128{2}) == xstd::div_result<xstd::int128>{.quotient = 4, .remainder = -1});
#endif

namespace {

auto show(std::int32_t numer, std::int32_t denom)
        -> void
{
        auto const [quotient, remainder] = xstd::div_ceil(numer, denom);
        std::cout << std::setw(2) << numer << " / " << std::setw(2) << denom << " = " << std::setw(2) << quotient << " remainder " << std::setw(2) << remainder << '\n';
}

} // namespace

auto main() -> int
{
        // The quotient rounds toward positive infinity, so a nonzero remainder takes the sign opposite to denom's.
        show(7, 2);
        show(-7, 2);
        show(7, -2);
        show(-7, -2);

        // The pages needed to show every item, ten to a page.
        constexpr auto per_page = std::int32_t{10};
        for (auto const items : {std::int32_t{0}, std::int32_t{20}, std::int32_t{23}}) {
                std::cout << std::setw(2) << items << " items fill " << xstd::div_ceil(items, per_page).quotient << " pages\n";
        }
}
