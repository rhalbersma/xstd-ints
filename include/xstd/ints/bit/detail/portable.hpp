//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_PORTABLE_HPP
#define XSTD_INTS_BIT_DETAIL_PORTABLE_HPP

// The constexpr kernels on one bit_limb, each the oracle its intrinsic counterpart must agree with.
#include <xstd/ints/bit/detail/portable/compress.hpp> // IWYU pragma: export; compress_by_rank
#include <xstd/ints/bit/detail/portable/expand.hpp>   // IWYU pragma: export; expand_by_select, expand_by_walk
#include <xstd/ints/bit/detail/portable/select.hpp>   // IWYU pragma: export; select, select_in_byte

#endif // XSTD_INTS_BIT_DETAIL_PORTABLE_HPP
