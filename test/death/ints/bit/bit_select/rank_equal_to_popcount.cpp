//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_select.hpp> // bit_select
#include <cstdint>                      // uint32_t

// Ranks count from zero, so the popcount names a one past the last, as counting from one would ask.
auto main() -> int
{
        auto const volatile rank = 3;
        static_cast<void>(xstd::bit_select(std::uint32_t{0b1011U}, rank));
}
