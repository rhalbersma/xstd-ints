//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory/align_down.hpp> // align_down
#include <xstd/ints/memory/align_up.hpp>   // align_up
#include <xstd/ints/memory/is_aligned.hpp> // is_aligned
#include <array>                           // array
#include <cstddef>                         // byte, size_t
#include <cstdint>                         // uint8_t, uint32_t
#include <iostream>                        // boolalpha, cout

static_assert(xstd::is_aligned(64UZ, 16));
static_assert(not xstd::is_aligned(24UZ, 16));
static_assert(xstd::is_aligned(std::uint8_t{0}, 128));

// What align_up and align_down return is aligned.
static_assert(xstd::is_aligned(xstd::align_up(std::uint32_t{1000}, 64), 64));
static_assert(xstd::is_aligned(xstd::align_down(std::uint32_t{1000}, 64), 64));

auto main() -> int
{
        std::cout << std::boolalpha;
        for (auto const offset : std::array<std::uint32_t, 4>{0, 12, 24, 40}) {
                std::cout << offset << " is 8-aligned: " << xstd::is_aligned(offset, 8) << '\n';
        }

        alignas(64) auto buffer = std::array<std::byte, 128>{};
        auto* const first       = buffer.data();
        std::cout << "first is 64-aligned: " << xstd::is_aligned(first, 64) << '\n';
        std::cout << "first + 8 is 16-aligned: " << xstd::is_aligned(first + 8, 16) << '\n';
}
