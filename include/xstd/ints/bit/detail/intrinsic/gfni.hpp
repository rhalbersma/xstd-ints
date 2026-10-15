//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_INTRINSIC_GFNI_HPP
#define XSTD_INTS_BIT_DETAIL_INTRINSIC_GFNI_HPP

// GF2P8AFFINEQB mirrors the bits of every byte in one instruction; 64-bit x86 alone moves a 64-bit lane to and fro.
#if (defined(__x86_64__) or defined(_M_X64)) and defined(__GFNI__)
#define XSTD_INTS_HAS_GFNI 1
#endif

#ifdef XSTD_INTS_HAS_GFNI
#include <immintrin.h> // _mm_cvtsi128_si32, _mm_cvtsi128_si64, _mm_cvtsi32_si128, _mm_cvtsi64_si128, _mm_gf2p8affine_epi64_epi8, _mm_set1_epi64x
#endif

#endif // XSTD_INTS_BIT_DETAIL_INTRINSIC_GFNI_HPP
