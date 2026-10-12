//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_PORTABLE_EXPAND_HPP
#define XSTD_INTS_DETAIL_PORTABLE_EXPAND_HPP

#include <xstd/ints/detail/bit_limb.hpp>        // bit_limb, bit_limb_width
#include <xstd/ints/detail/portable/select.hpp> // select
#include <bit>                                  // countr_zero, popcount

namespace xstd::ints::detail::portable {

// Each one-bit of the mask, lowest first, takes the next bit of src by masking rather than by a branch on that bit.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the source and then the mask
[[nodiscard]] constexpr auto expand_by_walk(bit_limb src, bit_limb mask) noexcept
        -> bit_limb
{
        auto dst = 0UZ;
        for (; mask != 0UZ; mask &= mask - 1UZ, src >>= 1U) {
                auto const lowest = mask & ~(mask - 1UZ);
                dst |= lowest & (0UZ - (src & 1UZ));
        }
        return dst;
}

// Each one-bit of src, at position j, lands on the mask's j-th one: one select per source bit, none per mask bit.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the source and then the mask
[[nodiscard]] constexpr auto expand_by_select(bit_limb src, bit_limb mask) noexcept
        -> bit_limb
{
        auto const mask_ones = static_cast<unsigned>(std::popcount(mask));
        if (mask_ones < bit_limb_width) {
                src &= (1UZ << mask_ones) - 1UZ;
        }
        auto dst = 0UZ;
        for (; src != 0UZ; src &= src - 1UZ) {
                dst |= 1UZ << select(mask, static_cast<unsigned>(std::countr_zero(src)));
        }
        return dst;
}

} // namespace xstd::ints::detail::portable

#endif // XSTD_INTS_DETAIL_PORTABLE_EXPAND_HPP
