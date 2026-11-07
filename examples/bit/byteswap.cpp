//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/byteswap.hpp>    // byteswap
#include <xstd/ints/cstdint/bit_int.hpp> // XSTD_HAS_BIT_INT, bit_uint
#include <xstd/ints/cstdint/int128.hpp>  // XSTD_HAS_INT128, uint128
#include <cstdint>                       // int16_t, int32_t, uint16_t, uint32_t, uint64_t
#include <iomanip>                       // hex, setfill, setw
#include <iostream>                      // cout

auto main() -> int
{
        constexpr auto word = std::uint32_t{0x1234'5678};
        constexpr auto wide = std::uint64_t{0x0102'0304'0506'0708};
        std::cout << std::hex << std::setfill('0');
        std::cout << std::setw(8) << word << " -> " << std::setw(8) << xstd::byteswap(word) << '\n';
        std::cout << std::setw(16) << wide << " -> " << std::setw(16) << xstd::byteswap(wide) << '\n';

        static_assert(xstd::byteswap(std::uint16_t{0xabcd}) == 0xcdab);
        static_assert(xstd::byteswap(std::int16_t{0x0080}) == std::int16_t{-0x8000});
        static_assert(xstd::byteswap(xstd::byteswap(std::int32_t{-2})) == -2);

#ifdef XSTD_HAS_INT128
        constexpr auto one = xstd::uint128{1};
        static_assert(xstd::byteswap(one) == static_cast<xstd::uint128>(one << 120));
#endif

#ifdef XSTD_HAS_BIT_INT
        // Three bytes of value in a four-byte object: the value's bytes are reversed, not the object's.
        static_assert(xstd::byteswap(xstd::bit_uint<24>{0x12'3456}) == xstd::bit_uint<24>{0x56'3412});
#endif
}
