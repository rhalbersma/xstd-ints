//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_COUNTL_ONE_HPP
#define XSTD_INTS_BIT_COUNTL_ONE_HPP

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/bit/countl_zero.hpp>                  // countl_zero
#include <bit>                                            // countl_one

// [bit.count] over every unsigned integer xstd knows.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::countl_one(x); }
[[nodiscard]] constexpr auto countl_one(T x) noexcept
        -> int
{
        return std::countl_one(x);
}

// The leading ones of x are the leading zeros of its complement.
template<unsigned_integer T>
        requires (not requires (T x) { std::countl_one(x); })
[[nodiscard]] constexpr auto countl_one(T x) noexcept(nothrow_const_operators<T>)
        -> int
{
        return xstd::countl_zero(static_cast<T>(~x));
}

} // namespace xstd

#endif // XSTD_INTS_BIT_COUNTL_ONE_HPP
