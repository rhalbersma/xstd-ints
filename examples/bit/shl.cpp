//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/shl.hpp>        // shl
#include <xstd/ints/bit/shr.hpp>        // shr
#include <xstd/ints/cstdint/int128.hpp> // XSTD_HAS_INT128, int128, uint128
#include <array>                        // array
#include <concepts>                     // same_as
#include <cstdint>                      // int8_t, uint8_t, uint32_t, uint64_t
#include <iomanip>                      // setw
#include <iostream>                     // cout

auto main() -> int
{
        constexpr auto u = std::uint8_t{12};
        constexpr auto i = std::int8_t{-12};
        std::cout << "  s  shl(u,s)  shr(u,s)  shl(i,s)  shr(i,s)\n";
        for (auto const s : std::array{-8, -3, -1, 0, 1, 3, 8}) {
                std::cout << std::setw(3) << s;
                std::cout << std::setw(10) << static_cast<int>(xstd::shl(u, s)) << std::setw(10) << static_cast<int>(xstd::shr(u, s));
                std::cout << std::setw(10) << static_cast<int>(xstd::shl(i, s)) << std::setw(10) << static_cast<int>(xstd::shr(i, s)) << '\n';
        }

        // The result keeps the type of the value shifted, and the count may be any integer type.
        static_assert(std::same_as<decltype(xstd::shl(u, 1)), std::uint8_t>);
        static_assert(xstd::shl(std::uint32_t{1}, std::uint64_t{1} << 40) == 0);
        static_assert(xstd::shr(std::int8_t{-5}, std::int8_t{1}) == -3);

#ifdef XSTD_HAS_INT128
        static_assert(xstd::shl(xstd::uint128{1}, 127) == static_cast<xstd::uint128>(xstd::uint128{1} << 127));
        static_assert(xstd::shr(xstd::int128{-1}, 1000) == xstd::int128{-1});
#endif
}
