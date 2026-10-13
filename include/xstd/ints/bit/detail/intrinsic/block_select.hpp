//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_INTRINSIC_BLOCK_SELECT_HPP
#define XSTD_INTS_BIT_DETAIL_INTRINSIC_BLOCK_SELECT_HPP

#include <xstd/ints/bit/detail/intrinsic/block_expand.hpp> // IWYU pragma: keep; block_expand, where PDEP is fast
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp>         // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor

#ifdef XSTD_INTS_HAS_FAST_BMI2

#include <bit>     // countr_zero, popcount
#include <cassert> // assert
#include <cstddef> // size_t
#include <utility> // cmp_less

namespace xstd::ints::detail::intrinsic {

// PDEP deposits the k-th bit of its source on the k-th one of its mask, so a lone k-th bit lands where select looks.
[[nodiscard]] inline auto block_select(std::size_t x, unsigned k) noexcept
        -> unsigned
{
        assert(std::cmp_less(k, std::popcount(x)));
        return static_cast<unsigned>(std::countr_zero(block_expand(std::size_t{1} << k, x)));
}

} // namespace xstd::ints::detail::intrinsic

#endif

#endif // XSTD_INTS_BIT_DETAIL_INTRINSIC_BLOCK_SELECT_HPP
