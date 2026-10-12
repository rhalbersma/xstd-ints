//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_INTRINSIC_COMPRESS_HPP
#define XSTD_INTS_DETAIL_INTRINSIC_COMPRESS_HPP

#include <xstd/ints/detail/intrinsic/bmi2.hpp> // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor

#ifdef XSTD_INTS_HAS_FAST_BMI2

namespace xstd::ints::detail::intrinsic {

// _pext_u32's own signature.
[[nodiscard]] inline auto compress(unsigned int x, unsigned int m) noexcept
        -> unsigned int
{
        return _pext_u32(x, m);
}

// _pext_u64's own signature.
[[nodiscard]] inline auto compress(unsigned long long x, unsigned long long m) noexcept
        -> unsigned long long
{
        return _pext_u64(x, m);
}

// A type of its own ([basic.fundamental]/2) as wide as one of the two above: 64 bits on LP64, 32 on LLP64.
[[nodiscard]] inline auto compress(unsigned long x, unsigned long m) noexcept
        -> unsigned long
{
        // Outside a template a discarded branch is still checked, so the requirement is stated once, before either.
        static_assert(sizeof(unsigned long) == sizeof(unsigned int) or sizeof(unsigned long) == sizeof(unsigned long long));
        if constexpr (sizeof(unsigned long) == sizeof(unsigned int)) {
                return static_cast<unsigned long>(compress(static_cast<unsigned int>(x), static_cast<unsigned int>(m)));
        } else {
                return static_cast<unsigned long>(compress(static_cast<unsigned long long>(x), static_cast<unsigned long long>(m)));
        }
}

} // namespace xstd::ints::detail::intrinsic

#endif

#endif // XSTD_INTS_DETAIL_INTRINSIC_COMPRESS_HPP
