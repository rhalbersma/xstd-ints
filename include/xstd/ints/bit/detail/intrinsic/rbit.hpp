//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_INTRINSIC_RBIT_HPP
#define XSTD_INTS_BIT_DETAIL_INTRINSIC_RBIT_HPP

// AArch64 mirrors a register in one RBIT, which ACLE spells __rbit and __rbitll; GCC defines no __ARM_ACLE to test.
#if defined(__aarch64__) and __has_include(<arm_acle.h>)
#define XSTD_INTS_HAS_RBIT 1
#endif

#ifdef XSTD_INTS_HAS_RBIT
#include <arm_acle.h> // __rbit, __rbitll
#endif

#endif // XSTD_INTS_BIT_DETAIL_INTRINSIC_RBIT_HPP
