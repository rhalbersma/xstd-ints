//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_ROTL_HPP
#define XSTD_INTS_BIT_ROTL_HPP

#include <xstd/ints/bit/detail/shift_count.hpp>           // shift_count_t
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // rotl

// [bit.rotate]'s rotl over every unsigned integer xstd knows.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x, int s) { std::rotl(x, s); }
[[nodiscard]] constexpr auto rotl(T x, int s) noexcept
        -> T
{
        return std::rotl(x, s);
}

// [bit.rotate]'s count taken modulo the width, a negative one turning the other way.
template<unsigned_integer T>
        requires (not requires (T x, int s) { std::rotl(x, s); })
[[nodiscard]] constexpr auto rotl(T x, int s) noexcept(nothrow_const_operators<T>)
        -> T
{
        using shift_count = ints::detail::shift_count_t<T>;
        constexpr auto N  = numeric_limits<T>::digits;
        auto const r      = s % N;
        if (r == 0) {
                return x;
        }
        auto const left = static_cast<shift_count>(r < 0 ? r + N : r);
        return static_cast<T>(static_cast<T>(x << left) | static_cast<T>(x >> (shift_count{N} - left)));
}

} // namespace xstd

#endif // XSTD_INTS_BIT_ROTL_HPP
