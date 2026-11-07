//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_expand.hpp> // bit_expand
#include <xstd/ints/bit/bit_repeat.hpp> // bit_repeat
#include <xstd/ints/cstdint/int128.hpp> // XSTD_HAS_INT128, uint128
#include <bitset>                       // bitset
#include <cstdint>                      // uint16_t, uint32_t
#include <iomanip>                      // setw
#include <iostream>                     // cout

// The Z-order index of a point: the bits of x and y interleaved, x on the odd positions and y on the even ones.
[[nodiscard]] constexpr auto morton(std::uint32_t x, std::uint32_t y)
        -> std::uint32_t
{
        constexpr auto odd_bits  = xstd::bit_repeat(std::uint32_t{0b10}, 2);
        constexpr auto even_bits = xstd::bit_repeat(std::uint32_t{0b01}, 2);
        return xstd::bit_expand(x, odd_bits) | xstd::bit_expand(y, even_bits);
}

auto main() -> int
{
        // The low bits of the source, deposited in their order on the positions of the mask's ones.
        constexpr auto green_mask = std::uint16_t{0b00000'111111'00000};
        std::cout << "green " << std::bitset<16>(xstd::bit_expand(std::uint16_t{0b101011}, green_mask)) << '\n';

        // The Z-order indices of a four-by-four grid, a row for each y.
        for (auto y = 0U; y < 4U; ++y) {
                for (auto x = 0U; x < 4U; ++x) {
                        std::cout << std::setw(3) << morton(x, y);
                }
                std::cout << '\n';
        }

        static_assert(xstd::bit_expand(std::uint16_t{0b1011}, std::uint16_t{0b0101}) == 0b0101);
        static_assert(morton(0xffff, 0) == 0xaaaa'aaaa);

#ifdef XSTD_HAS_INT128
        // The bits of a mask on either side of bit 64 receive adjacent bits of the source.
        constexpr auto straddle = static_cast<xstd::uint128>(xstd::uint128{0b11} << 63);
        static_assert(xstd::bit_expand(xstd::uint128{0b11}, straddle) == straddle);
#endif
}
