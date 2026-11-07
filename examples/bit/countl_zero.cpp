//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/countl_one.hpp>  // countl_one
#include <xstd/ints/bit/countl_zero.hpp> // countl_zero
#include <xstd/ints/bit/countr_one.hpp>  // countr_one
#include <xstd/ints/bit/countr_zero.hpp> // countr_zero
#include <xstd/ints/bit/popcount.hpp>    // popcount
#include <xstd/ints/cstdint/int128.hpp>  // XSTD_HAS_INT128, uint128
#include <array>                         // array
#include <bitset>                        // bitset
#include <cstdint>                       // uint8_t
#include <iomanip>                       // setw
#include <iostream>                      // cout

auto main() -> int
{
        std::cout << "x         clz clo ctz cto pop\n";
        for (auto const x : std::array<std::uint8_t, 4>{0b0000'0000, 0b0001'1100, 0b1110'0011, 0b1111'1111}) {
                std::cout << std::bitset<8>(x);
                std::cout << std::setw(5) << xstd::countl_zero(x) << std::setw(4) << xstd::countl_one(x);
                std::cout << std::setw(4) << xstd::countr_zero(x) << std::setw(4) << xstd::countr_one(x);
                std::cout << std::setw(4) << xstd::popcount(x) << '\n';
        }

#ifdef XSTD_HAS_INT128
        constexpr auto bit_64 = static_cast<xstd::uint128>(xstd::uint128{1} << 64);
        static_assert(xstd::countl_zero(bit_64) == 63);
        static_assert(xstd::countr_zero(bit_64) == 64);
        static_assert(xstd::countr_one(static_cast<xstd::uint128>(bit_64 - 1)) == 64);
        static_assert(xstd::popcount(static_cast<xstd::uint128>(~xstd::uint128{0})) == 128);
#endif
}
