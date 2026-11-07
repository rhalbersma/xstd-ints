//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory/align_down.hpp> // align_down
#include <array>                           // array
#include <cstddef>                         // byte, size_t
#include <cstdint>                         // uint8_t, uint64_t
#include <iostream>                        // cout, hex, showbase
#include <limits>                          // numeric_limits

static_assert(xstd::align_down(4095UZ, 4096) == 0);
static_assert(xstd::align_down(4096UZ, 4096) == 4096);
static_assert(xstd::align_down(std::uint8_t{255}, 16) == 240);

// Exact at the maximum, where rounding up would wrap.
constexpr auto max = std::numeric_limits<std::size_t>::max();
static_assert(xstd::align_down(max, 16) == max - 15);

auto main() -> int
{
        // The 4 KiB page an address falls in, and the offset into it.
        auto const address = std::uint64_t{0x7f3a'1234'5678};
        auto const page    = xstd::align_down(address, 4096);
        std::cout << std::hex << std::showbase;
        std::cout << "page   " << page << '\n';
        std::cout << "offset " << address - page << '\n';
        std::cout << std::dec << std::noshowbase;

        alignas(64) auto buffer = std::array<std::byte, 128>{};
        auto* const first       = buffer.data();
        std::cout << "first + 23 rounds down to first + " << xstd::align_down(first + 23, 16) - first << '\n';
}
