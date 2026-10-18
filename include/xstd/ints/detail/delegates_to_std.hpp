//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_DELEGATES_TO_STD_HPP
#define XSTD_INTS_DETAIL_DELEGATES_TO_STD_HPP

#include <xstd/ints/cstdint/int128.hpp>        // XSTD_HAS_INT128, uint128
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <concepts>                            // integral

namespace xstd::ints::detail {

#ifdef XSTD_HAS_INT128
using widest_std_unsigned = uint128;
#else
using widest_std_unsigned = unsigned long long;
#endif

// libc++ makes _BitInt integral, so ask the width, sign bit included: std::to_chars stops at its widest type.
template<class I>
inline constexpr auto delegates_to_std =
        std::integral<I> and (numeric_limits<I>::digits + (numeric_limits<I>::is_signed ? 1 : 0) <= numeric_limits<widest_std_unsigned>::digits);

} // namespace xstd::ints::detail

#endif // XSTD_INTS_DETAIL_DELEGATES_TO_STD_HPP
