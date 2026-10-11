//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_INTRINSIC_BMI2_HPP
#define XSTD_INTS_DETAIL_INTRINSIC_BMI2_HPP

// PEXT and PDEP are microcoded on AMD before Zen 3, so a build for one of those keeps the portable kernels.
#if (defined(__x86_64__) or defined(_M_X64)) and not defined(__bdver4__) and not defined(__znver1__) and not defined(__znver2__)
// MSVC never defines __BMI2__; every processor with AVX2 has BMI2, which /arch:AVX2 declares.
#if defined(__BMI2__) or (defined(_MSC_VER) and not defined(__clang__) and defined(__AVX2__))
#define XSTD_INTS_HAS_FAST_BMI2 1
#endif
#endif

#ifdef XSTD_INTS_HAS_FAST_BMI2
#include <immintrin.h> // _pdep_u64, _pext_u64
#endif

#endif // XSTD_INTS_DETAIL_INTRINSIC_BMI2_HPP
