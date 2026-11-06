//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_ROTR_HPP
#define XSTD_INTS_BIT_ROTR_HPP

#include <xstd/ints/detail/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/detail/numeric_limits.hpp>          // numeric_limits
#include <xstd/ints/detail/rotl.hpp>                    // rotl
#include <xstd/ints/detail/unsigned_integer.hpp>        // unsigned_integer
#include <bit>                                          // rotr

// [bit.rotate]'s rotr over every unsigned integer xstd knows.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x, int s) { std::rotr(x, s); }
[[nodiscard]] constexpr auto rotr(T x, int s) noexcept
        -> T
{
        return std::rotr(x, s);
}

// Rotating right is rotating left the other way, the count reduced first so that negating it cannot overflow.
template<unsigned_integer T>
        requires (not requires (T x, int s) { std::rotr(x, s); })
[[nodiscard]] constexpr auto rotr(T x, int s) noexcept(nothrow_const_operators<T>)
        -> T
{
        return xstd::rotl(x, -(s % numeric_limits<T>::digits));
}

} // namespace xstd

#endif // XSTD_INTS_BIT_ROTR_HPP
