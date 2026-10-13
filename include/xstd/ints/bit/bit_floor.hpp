//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_FLOOR_HPP
#define XSTD_INTS_BIT_BIT_FLOOR_HPP

#include <xstd/ints/bit/bit_width.hpp>                    // bit_width
#include <xstd/ints/bit/detail/shift_count.hpp>           // shift_count_t
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <bit>                                            // bit_floor

// [bit.pow.two]'s bit_floor over every unsigned integer xstd knows.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::bit_floor(x); }
[[nodiscard]] constexpr auto bit_floor(T x) noexcept
        -> T
{
        return std::bit_floor(x);
}

// The highest one on its own, and zero for zero.
template<unsigned_integer T>
        requires (not requires (T x) { std::bit_floor(x); })
[[nodiscard]] constexpr auto bit_floor(T x) noexcept(nothrow_const_operators<T>)
        -> T
{
        if (x == T{0}) {
                return T{0};
        }
        return static_cast<T>(T{1} << static_cast<ints::detail::shift_count_t<T>>(xstd::bit_width(x) - 1));
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_FLOOR_HPP
