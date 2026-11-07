//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_select.hpp>  // bit_select
#include <xstd/ints/bit/countr_zero.hpp> // countr_zero
#include <xstd/ints/bit/popcount.hpp>    // popcount
#include <xstd/ints/cstdint/int128.hpp>  // XSTD_HAS_INT128, uint128
#include <bitset>                        // bitset
#include <cstdint>                       // uint8_t
#include <iostream>                      // cout

auto main() -> int
{
        constexpr auto x = std::uint8_t{0b1010'0110};
        std::cout << "ones of " << std::bitset<8>(x) << '\n';
        for (auto k = 0; k < xstd::popcount(x); ++k) {
                std::cout << "rank " << k << " at bit " << xstd::bit_select(x, k) << '\n';
        }

        static_assert(xstd::bit_select(x, 0) == xstd::countr_zero(x));

#ifdef XSTD_HAS_INT128
        constexpr auto far_apart = static_cast<xstd::uint128>((xstd::uint128{1} << 100) | xstd::uint128{1});
        static_assert(xstd::bit_select(far_apart, 1) == 100);
#endif
}
