//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_PORTABLE_BLOCK_EXPAND_HPP
#define XSTD_INTS_BIT_DETAIL_PORTABLE_BLOCK_EXPAND_HPP

#include <xstd/ints/bit/detail/block_width.hpp>     // block_width
#include <xstd/ints/bit/detail/portable/select.hpp> // select
#include <bit>                                      // countr_zero, popcount
#include <cstddef>                                  // size_t

namespace xstd::ints::detail::portable {

// Each one-bit of the mask, lowest first, takes the next bit of src by masking rather than by a branch on that bit.
[[nodiscard]] constexpr auto block_expand_by_walk(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        auto dst = 0UZ;
        for (; mask != 0UZ; mask &= mask - 1UZ, src >>= 1U) {
                auto const lowest = mask & ~(mask - 1UZ);
                dst |= lowest & (0UZ - (src & 1UZ));
        }
        return dst;
}

// Each one-bit of src, at position j, lands on the mask's j-th one: one select per source bit, none per mask bit.
[[nodiscard]] constexpr auto block_expand_by_select(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        auto const mask_ones = static_cast<unsigned>(std::popcount(mask));
        if (mask_ones < block_width) {
                src &= (1UZ << mask_ones) - 1UZ;
        }
        auto dst = 0UZ;
        for (; src != 0UZ; src &= src - 1UZ) {
                dst |= 1UZ << select(mask, static_cast<unsigned>(std::countr_zero(src)));
        }
        return dst;
}

} // namespace xstd::ints::detail::portable

#endif // XSTD_INTS_BIT_DETAIL_PORTABLE_BLOCK_EXPAND_HPP
