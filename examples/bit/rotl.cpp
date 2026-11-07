//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/rotl.hpp>       // rotl
#include <xstd/ints/bit/rotr.hpp>       // rotr
#include <xstd/ints/cstdint/int128.hpp> // XSTD_HAS_INT128, uint128
#include <bitset>                       // bitset
#include <cstdint>                      // uint8_t
#include <iostream>                     // cout
#include <limits>                       // numeric_limits

auto main() -> int
{
        constexpr auto x = std::uint8_t{0b1001'0110};
        std::cout << "x          " << std::bitset<8>(x) << '\n';
        std::cout << "rotl(x, 1) " << std::bitset<8>(xstd::rotl(x, 1)) << '\n';
        std::cout << "rotl(x, 3) " << std::bitset<8>(xstd::rotl(x, 3)) << '\n';
        std::cout << "rotr(x, 1) " << std::bitset<8>(xstd::rotr(x, 1)) << '\n';
        std::cout << "rotr(x, 9) " << std::bitset<8>(xstd::rotr(x, 9)) << '\n';

        // A negative count turns the other way, and every int count is in range.
        static_assert(xstd::rotl(x, -1) == xstd::rotr(x, 1));
        static_assert(xstd::rotr(xstd::rotl(x, std::numeric_limits<int>::max()), std::numeric_limits<int>::max()) == x);

#ifdef XSTD_HAS_INT128
        constexpr auto top = static_cast<xstd::uint128>(xstd::uint128{1} << 127);
        static_assert(xstd::rotl(top, 1) == xstd::uint128{1});
        static_assert(xstd::rotr(xstd::uint128{1}, 1) == top);
#endif
}
