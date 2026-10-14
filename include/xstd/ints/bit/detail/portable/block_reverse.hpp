//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_PORTABLE_BLOCK_REVERSE_HPP
#define XSTD_INTS_BIT_DETAIL_PORTABLE_BLOCK_REVERSE_HPP

#include <bit>     // byteswap
#include <cstddef> // size_t

namespace xstd::ints::detail::portable {

// Mirroring the bytes leaves each byte's own bits to mirror: its nibbles, then its pairs, then its single bits.
[[nodiscard]] constexpr auto block_reverse_by_swaps(std::size_t x) noexcept
        -> std::size_t
{
        constexpr auto nibbles = ~0UZ / 0x11UZ;
        constexpr auto pairs   = ~0UZ / 0x05UZ;
        constexpr auto singles = ~0UZ / 0x03UZ;
        x                      = std::byteswap(x);
        x                      = ((x >> 4U) & nibbles) | ((x & nibbles) << 4U);
        x                      = ((x >> 2U) & pairs) | ((x & pairs) << 2U);
        x                      = ((x >> 1U) & singles) | ((x & singles) << 1U);
        return x;
}

} // namespace xstd::ints::detail::portable

#endif // XSTD_INTS_BIT_DETAIL_PORTABLE_BLOCK_REVERSE_HPP
