//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_BLOCK_SELECT_HPP
#define XSTD_INTS_BIT_DETAIL_BLOCK_SELECT_HPP

#include <xstd/ints/bit/detail/intrinsic/block_select.hpp> // IWYU pragma: keep; block_select, where the target makes PDEP fast
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp>         // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/bit/detail/portable/block_select.hpp>  // block_select_by_broadword
#include <cstddef>                                         // size_t

namespace xstd::ints::detail {

// The hardware instruction at run time where the target makes it fast, the portable kernel everywhere else.
[[nodiscard]] constexpr auto block_select(std::size_t x, unsigned k) noexcept
        -> unsigned
{
#ifdef XSTD_INTS_HAS_FAST_BMI2
        if not consteval {
                return intrinsic::block_select(x, k);
        }
#endif
        return portable::block_select_by_broadword(x, k);
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_BLOCK_SELECT_HPP
