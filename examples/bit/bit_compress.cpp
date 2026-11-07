//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_compress.hpp> // bit_compress
#include <xstd/ints/cstdint/int128.hpp>   // XSTD_HAS_INT128, uint128
#include <bitset>                         // bitset
#include <concepts>                       // same_as
#include <cstdint>                        // uint16_t
#include <iostream>                       // cout

auto main() -> int
{
        // An RGB565 pixel: five bits of red, six of green, five of blue.
        constexpr auto pixel      = std::uint16_t{0b10110'011010'01101};
        constexpr auto red_mask   = std::uint16_t{0b11111'000000'00000};
        constexpr auto green_mask = std::uint16_t{0b00000'111111'00000};
        constexpr auto blue_mask  = std::uint16_t{0b00000'000000'11111};

        std::cout << "pixel " << std::bitset<16>(pixel) << '\n';
        std::cout << "red   " << std::bitset<16>(xstd::bit_compress(pixel, red_mask)) << '\n';
        std::cout << "green " << std::bitset<16>(xstd::bit_compress(pixel, green_mask)) << '\n';
        std::cout << "blue  " << std::bitset<16>(xstd::bit_compress(pixel, blue_mask)) << '\n';

        // A scattered mask packs the bits it selects side by side, in their order.
        constexpr auto even_bits = std::uint16_t{0b0101'0101'0101'0101};
        std::cout << "even  " << std::bitset<16>(xstd::bit_compress(pixel, even_bits)) << '\n';

        static_assert(xstd::bit_compress(std::uint16_t{0b1011}, std::uint16_t{0b0101}) == 0b01);
        static_assert(std::same_as<decltype(xstd::bit_compress(pixel, red_mask)), std::uint16_t>);

#ifdef XSTD_HAS_INT128
        // Selected bits on either side of bit 64 land next to each other.
        constexpr auto straddle = static_cast<xstd::uint128>(xstd::uint128{0b11} << 63);
        static_assert(xstd::bit_compress(straddle, straddle) == xstd::uint128{0b11});
#endif
}
