//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_COMPRESS_LIMB_HPP
#define XSTD_INTS_BIT_DETAIL_COMPRESS_LIMB_HPP

#include <xstd/ints/bit/detail/bit_limb.hpp>           // bit_limb
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp>     // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/bit/detail/intrinsic/compress.hpp> // IWYU pragma: keep; compress, where the target makes PEXT fast
#include <xstd/ints/bit/detail/portable/compress.hpp>  // compress_by_rank

namespace xstd::ints::detail {

// The hardware instruction at run time where the target makes it fast, the portable kernel everywhere else.
[[nodiscard]] constexpr auto compress_limb(bit_limb src, bit_limb mask) noexcept
        -> bit_limb
{
#ifdef XSTD_INTS_HAS_FAST_BMI2
        if not consteval {
                return intrinsic::compress(src, mask);
        }
#endif
        return portable::compress_by_rank(src, mask);
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_COMPRESS_LIMB_HPP
