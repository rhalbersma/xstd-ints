//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_STD_PERMUTES_BITS_HPP
#define XSTD_INTS_DETAIL_STD_PERMUTES_BITS_HPP

#include <bit>     // IWYU pragma: keep; bit_compress, bit_expand, bit_repeat, bit_reverse, where the standard library ships them
#include <version> // IWYU pragma: keep; __cpp_lib_bitops

// The working draft's __cpp_lib_bitops for P3104, which puts the four permutations in <bit>.
#if defined(__cpp_lib_bitops) and __cpp_lib_bitops >= 202607L
#define XSTD_HAS_STD_BIT_PERMUTATIONS 1
#endif

namespace xstd::ints::detail {

#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS

template<class T>
concept std_permutes_bits = requires (T x, int l) {
        std::bit_compress(x, x);
        std::bit_expand(x, x);
        std::bit_repeat(x, l);
        std::bit_reverse(x);
};

#else

template<class T>
concept std_permutes_bits = false;

#endif

} // namespace xstd::ints::detail

#endif // XSTD_INTS_DETAIL_STD_PERMUTES_BITS_HPP
