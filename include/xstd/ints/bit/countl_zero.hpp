//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_COUNTL_ZERO_HPP
#define XSTD_INTS_BIT_COUNTL_ZERO_HPP

#include <bit> // countl_zero

// One overload set per function: <bit> takes std::unsigned_integral, which every 128-bit integer class fails.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::countl_zero(x); }
[[nodiscard]] constexpr auto countl_zero(T x) noexcept
        -> int
{
        return std::countl_zero(x);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_COUNTL_ZERO_HPP
