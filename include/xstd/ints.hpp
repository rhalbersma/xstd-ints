//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_HPP
#define XSTD_INTS_HPP

// Not the ext adaptors: each hard-includes what it adapts, so a front door naming them would put Abseil on every path.
#include <xstd/ints/bit.hpp>         // IWYU pragma: export; countl_zero, countr_zero, popcount
#include <xstd/ints/charconv.hpp>    // IWYU pragma: export; to_chars, to_chars_max_size
#include <xstd/ints/concepts.hpp>    // IWYU pragma: export; the integer concepts
#include <xstd/ints/cstdint.hpp>     // IWYU pragma: export; bit_int, bit_uint, int128, uint128
#include <xstd/ints/cstdlib.hpp>     // IWYU pragma: export; the integer functions
#include <xstd/ints/format.hpp>      // IWYU pragma: export; formatter<div_result>
#include <xstd/ints/limits.hpp>      // IWYU pragma: export; numeric_limits
#include <xstd/ints/memory.hpp>      // IWYU pragma: export; align_up, align_down, is_aligned
#include <xstd/ints/type_traits.hpp> // IWYU pragma: export; the sign traits and modifiers

#endif // XSTD_INTS_HPP
