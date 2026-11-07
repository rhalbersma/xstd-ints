//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_reverse.hpp> // bit_reverse
#include <xstd/ints/cstdint/int128.hpp>  // XSTD_HAS_INT128, uint128
#include <bitset>                        // bitset
#include <cstdint>                       // uint8_t, uint32_t
#include <iostream>                      // cout

auto main() -> int
{
        constexpr auto x = std::uint8_t{0b1101'0010};
        std::cout << std::bitset<8>(x) << " reversed is " << std::bitset<8>(xstd::bit_reverse(x)) << '\n';

        // The bit-reversal permutation of an eight-point FFT: each three-bit index mirrored.
        std::cout << "FFT order:";
        for (auto i = 0U; i < 8U; ++i) {
                std::cout << ' ' << (xstd::bit_reverse(static_cast<std::uint8_t>(i)) >> 5);
        }
        std::cout << '\n';

        static_assert(xstd::bit_reverse(std::uint32_t{0x0000'1234}) == 0x2c48'0000);
        static_assert(xstd::bit_reverse(xstd::bit_reverse(x)) == x);

#ifdef XSTD_HAS_INT128
        static_assert(xstd::bit_reverse(xstd::uint128{1}) == static_cast<xstd::uint128>(xstd::uint128{1} << 127));
#endif
}
