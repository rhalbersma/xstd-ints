//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_WIDTH_HPP
#define XSTD_INTS_BIT_BIT_WIDTH_HPP

#include <xstd/ints/bit/countl_zero.hpp>                  // countl_zero
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // bit_width

// [bit.pow.two]'s bit_width over every unsigned integer xstd knows, an int as LWG 3656 made it.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::bit_width(x); }
[[nodiscard]] constexpr auto bit_width(T x) noexcept
        -> int
{
        return std::bit_width(x);
}

// The bits below and including the highest one.
template<unsigned_integer T>
        requires (not requires (T x) { std::bit_width(x); })
[[nodiscard]] constexpr auto bit_width(T x) noexcept(nothrow_const_operators<T>)
        -> int
{
        return numeric_limits<T>::digits - xstd::countl_zero(x);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_WIDTH_HPP
