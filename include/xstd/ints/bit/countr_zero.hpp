//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_COUNTR_ZERO_HPP
#define XSTD_INTS_BIT_COUNTR_ZERO_HPP

#include <bit> // countr_zero

// See bit/popcount.hpp for why the 128-bit integer classes carry their own overloads.
namespace xstd {

// Constrained on the call itself, so constraint and body cannot drift: std::unsigned_integral admits four types <bit> refuses, and on libc++ carries _BitInt in while <bit> declines it (P3666R4).
template<class T>
        requires requires (T x) { std::countr_zero(x); }
[[nodiscard]] constexpr auto countr_zero(T x) noexcept
        -> int
{
        return std::countr_zero(x);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_COUNTR_ZERO_HPP
