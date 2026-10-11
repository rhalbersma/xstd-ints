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

// Each one-bit of the mask, lowest first, takes the next bit of x by masking rather than by a branch on that bit.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] constexpr auto expand_by_walk(bit_limb x, bit_limb m) noexcept
        -> bit_limb
{
        auto result = bit_limb{0};
        for (; m != bit_limb{0}; m &= m - 1U, x >>= 1U) {
                auto const lowest = m & ~(m - 1U);
                result |= lowest & (bit_limb{0} - (x & 1U));
        }
        return result;
}

// Each one-bit of x, at position j, lands on the mask's j-th one: one select per source bit, none per mask bit.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] constexpr auto expand_by_select(bit_limb x, bit_limb m) noexcept
        -> bit_limb
{
        auto const mask_ones = static_cast<unsigned>(std::popcount(m));
        if (mask_ones < bit_limb_width) {
                x &= (bit_limb{1} << mask_ones) - 1U;
        }
        auto result = bit_limb{0};
        for (; x != bit_limb{0}; x &= x - 1U) {
                result |= bit_limb{1} << select(m, static_cast<unsigned>(std::countr_zero(x)));
        }
        return result;
}

} // namespace xstd::ints::detail::portable

#endif // XSTD_INTS_DETAIL_PORTABLE_EXPAND_HPP
