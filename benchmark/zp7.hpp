//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BENCH_ZP7_HPP
#define XSTD_INTS_BENCH_ZP7_HPP

// zp7.c's own includes, made here at global scope so that its includes inside the namespaces below are no-ops.
#include <stdint.h> // NOLINT(modernize-deprecated-headers): uint64_t, as zp7.c spells it

#if defined(__PCLMUL__) or defined(__POPCNT__) or defined(__BMI2__)
#include <immintrin.h> // _bzhi_u64, _mm_and_si128, _mm_clmulepi64_si128, _mm_cvtsi128_si64, _mm_cvtsi64_si128
#include <x86intrin.h> // _popcnt64, which Clang declares here rather than in immintrin.h
#endif

// Wegner's zp7 twice, as its source defines it, so that one translation unit times it with and without PCLMULQDQ.
#ifdef __POPCNT__
#define HAS_POPCNT
#endif
#ifdef __BMI2__
#define HAS_BZHI
#endif

namespace bench::zp7_by_shifts {
#include <zp7.c>
} // namespace bench::zp7_by_shifts

#ifdef __PCLMUL__
#define HAS_CLMUL

namespace bench::zp7_by_clmul {
#include <zp7.c>
} // namespace bench::zp7_by_clmul

#undef HAS_CLMUL
#endif

#undef HAS_POPCNT
#undef HAS_BZHI

#endif // XSTD_INTS_BENCH_ZP7_HPP
