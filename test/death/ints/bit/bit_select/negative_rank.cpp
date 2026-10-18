//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_select.hpp> // bit_select
#include <cstdint>                      // uint32_t

// Minus one, the index of the last element in some languages, names no rank here.
auto main() -> int
{
        auto const volatile rank = -1;
        static_cast<void>(xstd::bit_select(std::uint32_t{0b1011U}, rank));
}
