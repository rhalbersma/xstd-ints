//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>  // uint128, XSTD_HAS_INT128
#include <xstd/ints/memory/align_up.hpp> // align_up
#include <array>                         // array
#include <concepts>                      // same_as
#include <cstddef>                       // byte, size_t
#include <cstdint>                       // uint8_t, uint32_t
#include <iostream>                      // cout

static_assert(xstd::align_up(13UZ, 8) == 16);
static_assert(xstd::align_up(16UZ, 8) == 16);
static_assert(noexcept(xstd::align_up(13UZ, 8)));

// A narrow type is returned as itself, not promoted to int.
static_assert(std::same_as<decltype(xstd::align_up(std::uint8_t{250}, 4)), std::uint8_t>);

#ifdef XSTD_HAS_INT128
static_assert(xstd::align_up(xstd::uint128{1} << 100U, 4096) == xstd::uint128{1} << 100U);
#endif

auto main() -> int
{
        // The offsets of three fields laid out one after another, each at its own alignment.
        auto offset = std::uint32_t{0};
        for (auto const [size, alignment] : std::array<std::array<std::uint32_t, 2>, 3>{{{1, 1}, {8, 8}, {2, 2}}}) {
                offset = xstd::align_up(offset, alignment);
                std::cout << "field at " << offset << '\n';
                offset += size;
        }
        std::cout << "size " << xstd::align_up(offset, 8) << '\n';

        alignas(64) auto buffer = std::array<std::byte, 128>{};
        auto* const first       = buffer.data();
        std::cout << "first + 1 rounds up to first + " << xstd::align_up(first + 1, 16) - first << '\n';
        std::cout << "first + 32 stays at first + " << xstd::align_up(first + 32, 16) - first << '\n';
}
