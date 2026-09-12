//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_POPCOUNT_HPP
#define XSTD_INTS_BIT_POPCOUNT_HPP

#include <bit> // popcount

// The bit basis, one overload set per function: <bit> takes std::unsigned_integral, which every 128-bit integer CLASS fails, so each carries its own overload beside its header.
namespace xstd {

// Constrained on the call itself, so constraint and body cannot drift: std::unsigned_integral admits four types <bit> refuses, and on libc++ carries _BitInt in while <bit> declines it (P3666R4).
template<class T>
        requires requires (T x) { std::popcount(x); }
[[nodiscard]] constexpr auto popcount(T x) noexcept
        -> int
{
        return std::popcount(x);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_POPCOUNT_HPP
