//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDLIB_DIV_CEIL_HPP
#define XSTD_INTS_CSTDLIB_DIV_CEIL_HPP

#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdlib/div.hpp>                      // div
#include <xstd/ints/cstdlib/div_result.hpp>               // IWYU pragma: export; div_result
#include <xstd/ints/cstdlib/sign.hpp>                     // sign
#include <xstd/ints/cstdlib/unsigned_abs.hpp>             // unsigned_abs
#include <xstd/ints/type_traits/is_unsigned.hpp>          // is_unsigned_v
#include <cassert>                                        // assert

namespace xstd {

// Ceiling division: a nonzero remainder has the sign opposite the denominator's, modulo 2^N where unsigned.
template<integer I>
[[nodiscard]] constexpr auto div_ceil(I numer, I denom) noexcept(nothrow_const_operators<I>)
        -> div_result<I>
{
        assert(denom != static_cast<I>(0));
        // Qualified: unqualified, ADL finds Boost.Int128's own div and it wins.
        auto const [qT, rT] = xstd::div(numer, denom);
        auto const zero     = static_cast<I>(0);
        auto const one      = static_cast<I>(1);
        if constexpr (is_unsigned_v<I>) {
                // Any remainder rounds up, leaving a negative remainder that wraps as P3724R4 words it.
                auto const adjust = rT != zero;
                return {.quotient = static_cast<I>(qT + (adjust ? one : zero)), .remainder = static_cast<I>(rT - (adjust ? denom : zero))};
        } else {
                // A positive inexact quotient rounds up: the truncated remainder then has the denominator's sign.
                auto const adjust = xstd::sign(rT) == xstd::sign(denom);
                auto const qC     = static_cast<I>(qT + (adjust ? one : zero));
                auto const rC     = static_cast<I>(rT - (adjust ? denom : zero));
                // Said on the counterpart every integer type has, |MIN| fitting in no other.
                assert(xstd::unsigned_abs(rC) < xstd::unsigned_abs(denom));
                assert(xstd::sign(rC) == -xstd::sign(denom) or rC == zero);
                return {.quotient = qC, .remainder = rC};
        }
}

} // namespace xstd

#endif // XSTD_INTS_CSTDLIB_DIV_CEIL_HPP
