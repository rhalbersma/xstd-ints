//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_STD_SHIFTS_HPP
#define XSTD_INTS_DETAIL_STD_SHIFTS_HPP

#include <bit>     // IWYU pragma: keep; shl, shr, where the standard library ships them
#include <version> // IWYU pragma: keep; __cpp_lib_bitops

// The working draft's __cpp_lib_bitops for P3793, which puts shl and shr in <bit>.
#if defined(__cpp_lib_bitops) and __cpp_lib_bitops >= 202606L
#define XSTD_HAS_STD_SHIFTS 1
#endif

namespace xstd::ints::detail {

#ifdef XSTD_HAS_STD_SHIFTS

template<class T, class S>
concept std_shifts = requires (T x, S s) {
        std::shl(x, s);
        std::shr(x, s);
};

#else

template<class T, class S>
concept std_shifts = false;

#endif

} // namespace xstd::ints::detail

#endif // XSTD_INTS_DETAIL_STD_SHIFTS_HPP
