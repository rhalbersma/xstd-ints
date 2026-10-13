//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_BLOCK_COMPRESS_HPP
#define XSTD_INTS_BIT_DETAIL_BLOCK_COMPRESS_HPP

#include <xstd/ints/bit/detail/intrinsic/block_compress.hpp> // IWYU pragma: keep; compress, where the target makes PEXT fast
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp>           // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/bit/detail/portable/block_compress.hpp>  // block_compress_by_rank
#include <cstddef>                                           // size_t

namespace xstd::ints::detail {

// The hardware instruction at run time where the target makes it fast, the portable kernel everywhere else.
[[nodiscard]] constexpr auto block_compress(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
#ifdef XSTD_INTS_HAS_FAST_BMI2
        if not consteval {
                return intrinsic::block_compress(src, mask);
        }
#endif
        return portable::block_compress_by_rank(src, mask);
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_BLOCK_COMPRESS_HPP
