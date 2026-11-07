//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_repeat.hpp> // bit_repeat
#include <xstd/ints/cstdint/int128.hpp> // XSTD_HAS_INT128, uint128
#include <cstdint>                      // uint8_t, uint32_t, uint64_t
#include <iomanip>                      // hex, setfill, setw
#include <iostream>                     // cout

auto main() -> int
{
        // The masks of a parallel bit count: runs of i ones and i zeros, repeated across all 64 bits.
        for (auto i = 1; i < 64; i *= 2) {
                auto const mask = xstd::bit_repeat(static_cast<std::uint64_t>((std::uint64_t{1} << i) - 1), 2 * i);
                std::cout << "period " << std::setw(2) << std::setfill(' ') << 2 * i << ": ";
                std::cout << std::hex << std::setw(16) << std::setfill('0') << mask << std::dec << '\n';
        }

        static_assert(xstd::bit_repeat(std::uint32_t{0xc}, 4) == 0xcccc'cccc);
        static_assert(xstd::bit_repeat(std::uint8_t{0b1}, 1) == 0xff);
        static_assert(xstd::bit_repeat(std::uint8_t{0x5a}, 8) == 0x5a);

#ifdef XSTD_HAS_INT128
        constexpr auto low_half = xstd::uint128{0xcccc'cccc'cccc'cccc};
        static_assert(xstd::bit_repeat(xstd::uint128{0xc}, 4) == (static_cast<xstd::uint128>(low_half << 64) | low_half));
#endif
}
