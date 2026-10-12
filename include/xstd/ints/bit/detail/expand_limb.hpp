//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_EXPAND_LIMB_HPP
#define XSTD_INTS_BIT_DETAIL_EXPAND_LIMB_HPP

#include <xstd/ints/bit/detail/bit_limb.hpp>         // bit_limb
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp>   // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/bit/detail/intrinsic/expand.hpp> // IWYU pragma: keep; expand, where the target makes PDEP fast
#include <xstd/ints/bit/detail/portable/expand.hpp>  // expand_by_walk

namespace xstd::ints::detail {

// The hardware instruction at run time where the target makes it fast, the portable kernel everywhere else.
[[nodiscard]] constexpr auto expand_limb(bit_limb src, bit_limb mask) noexcept
        -> bit_limb
{
#ifdef XSTD_INTS_HAS_FAST_BMI2
        if not consteval {
                return intrinsic::expand(src, mask);
        }
#endif
        return portable::expand_by_walk(src, mask);
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_EXPAND_LIMB_HPP
