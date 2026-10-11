//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_INTRINSIC_HPP
#define XSTD_INTS_DETAIL_INTRINSIC_HPP

// The hardware kernels on one bit_limb, each declared only where the target makes it fast.
#include <xstd/ints/detail/intrinsic/bmi2.hpp>     // IWYU pragma: export; XSTD_INTS_HAS_FAST_BMI2
#include <xstd/ints/detail/intrinsic/compress.hpp> // IWYU pragma: export; compress
#include <xstd/ints/detail/intrinsic/expand.hpp>   // IWYU pragma: export; expand

#endif // XSTD_INTS_DETAIL_INTRINSIC_HPP
