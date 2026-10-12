//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_INTRINSIC_EXPAND_HPP
#define XSTD_INTS_DETAIL_INTRINSIC_EXPAND_HPP

#include <xstd/ints/detail/intrinsic/bmi2.hpp> // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor

#ifdef XSTD_INTS_HAS_FAST_BMI2

namespace xstd::ints::detail::intrinsic {

// _pdep_u32's own signature.
[[nodiscard]] inline auto expand(unsigned int src, unsigned int mask) noexcept
        -> unsigned int
{
        return _pdep_u32(src, mask);
}

// _pdep_u64's own signature.
[[nodiscard]] inline auto expand(unsigned long long src, unsigned long long mask) noexcept
        -> unsigned long long
{
        return _pdep_u64(src, mask);
}

// A type of its own ([basic.fundamental]/2) as wide as one of the two above: 64 bits on LP64, 32 on LLP64.
[[nodiscard]] inline auto expand(unsigned long src, unsigned long mask) noexcept
        -> unsigned long
{
        // Outside a template a discarded branch is still checked, so the requirement is stated once, before either.
        static_assert(sizeof(unsigned long) == sizeof(unsigned int) or sizeof(unsigned long) == sizeof(unsigned long long));
        if constexpr (sizeof(unsigned long) == sizeof(unsigned int)) {
                return static_cast<unsigned long>(expand(static_cast<unsigned int>(src), static_cast<unsigned int>(mask)));
        } else {
                return static_cast<unsigned long>(expand(static_cast<unsigned long long>(src), static_cast<unsigned long long>(mask)));
        }
}

} // namespace xstd::ints::detail::intrinsic

#endif

#endif // XSTD_INTS_DETAIL_INTRINSIC_EXPAND_HPP
