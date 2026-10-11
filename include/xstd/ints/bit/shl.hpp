//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_SHL_HPP
#define XSTD_INTS_BIT_SHL_HPP

#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/detail/bit_shift.hpp>                 // XSTD_HAS_STD_SHIFTS, shift, shift_direction, std_shifts
#include <bit>                                            // IWYU pragma: keep; shl, where the standard library ships it

// P3793R2's shl over every pair of integers xstd knows: any count is in range, a negative one shifting the other way.
namespace xstd {

#ifdef XSTD_HAS_STD_SHIFTS

template<class T, class S>
        requires ints::detail::std_shifts<T, S>
[[nodiscard]] constexpr auto shl(T x, S s) noexcept
        -> T
{
        return std::shl(x, s);
}

#endif

template<integer T, integer S>
        requires (not ints::detail::std_shifts<T, S>)
[[nodiscard]] constexpr auto shl(T x, S s) noexcept(nothrow_const_operators<T> and nothrow_const_operators<S>)
        -> T
{
        return ints::detail::shift(x, s, ints::detail::shift_direction::left);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_SHL_HPP
