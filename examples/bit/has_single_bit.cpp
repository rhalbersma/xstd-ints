//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_ceil.hpp>       // bit_ceil
#include <xstd/ints/bit/bit_floor.hpp>      // bit_floor
#include <xstd/ints/bit/bit_width.hpp>      // bit_width
#include <xstd/ints/bit/has_single_bit.hpp> // has_single_bit
#include <xstd/ints/cstdint/int128.hpp>     // XSTD_HAS_INT128, uint128
#include <array>                            // array
#include <cstdint>                          // uint32_t
#include <iomanip>                          // boolalpha, setw
#include <iostream>                         // cout

auto main() -> int
{
        std::cout << "  x  single  floor  ceil  width\n";
        for (auto const x : std::array<std::uint32_t, 6>{0, 1, 5, 8, 12, 100}) {
                std::cout << std::setw(3) << x << std::setw(8) << std::boolalpha << xstd::has_single_bit(x);
                std::cout << std::setw(7) << xstd::bit_floor(x) << std::setw(6) << xstd::bit_ceil(x);
                std::cout << std::setw(7) << xstd::bit_width(x) << '\n';
        }

#ifdef XSTD_HAS_INT128
        constexpr auto two_to_the_100 = static_cast<xstd::uint128>(xstd::uint128{1} << 100);
        static_assert(xstd::has_single_bit(two_to_the_100));
        static_assert(xstd::bit_ceil(static_cast<xstd::uint128>(two_to_the_100 + 1)) == static_cast<xstd::uint128>(two_to_the_100 << 1));
        static_assert(xstd::bit_floor(static_cast<xstd::uint128>(two_to_the_100 + 1)) == two_to_the_100);
        static_assert(xstd::bit_width(two_to_the_100) == 101);
#endif
}
