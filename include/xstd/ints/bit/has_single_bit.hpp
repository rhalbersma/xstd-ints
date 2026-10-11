//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_HAS_SINGLE_BIT_HPP
#define XSTD_INTS_BIT_HAS_SINGLE_BIT_HPP

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <bit>                                            // has_single_bit

// [bit.pow.two]'s has_single_bit over every unsigned integer xstd knows.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::has_single_bit(x); }
[[nodiscard]] constexpr auto has_single_bit(T x) noexcept
        -> bool
{
        return std::has_single_bit(x);
}

// Clearing the lowest one leaves nothing exactly when it was the only one.
template<unsigned_integer T>
        requires (not requires (T x) { std::has_single_bit(x); })
[[nodiscard]] constexpr auto has_single_bit(T x) noexcept(nothrow_const_operators<T>)
        -> bool
{
        return x != T{0} and static_cast<T>(x & static_cast<T>(x - T{1})) == T{0};
}

} // namespace xstd

#endif // XSTD_INTS_BIT_HAS_SINGLE_BIT_HPP
