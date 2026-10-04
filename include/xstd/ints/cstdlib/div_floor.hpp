//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDLIB_DIV_FLOOR_HPP
#define XSTD_INTS_CSTDLIB_DIV_FLOOR_HPP

#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdlib/div.hpp>                      // div
#include <xstd/ints/cstdlib/div_result.hpp>               // IWYU pragma: export; div_result
#include <xstd/ints/cstdlib/sign.hpp>                     // sign
#include <xstd/ints/cstdlib/unsigned_abs.hpp>             // unsigned_abs
#include <xstd/ints/type_traits/is_unsigned.hpp>          // is_unsigned_v
#include <cassert>                                        // assert

namespace xstd {

// Floored division: a nonzero remainder has the denominator's sign.
template<integer I>
[[nodiscard]] constexpr auto div_floor(I numer, I denom) noexcept(nothrow_const_operators<I>)
        -> div_result<I>
{
        assert(denom != static_cast<I>(0));
        // An unsigned remainder cannot disagree in sign with its denominator.
        if constexpr (is_unsigned_v<I>) {
                // Qualified: unqualified, ADL finds Boost.Int128's own div and it wins.
                return xstd::div(numer, denom);
        } else {
                auto const [qT, rT] = xstd::div(numer, denom);
                auto const zero     = static_cast<I>(0);
                auto const one      = static_cast<I>(1);
                auto const adjust   = xstd::sign(rT) == -xstd::sign(denom);
                auto const qF       = static_cast<I>(qT - (adjust ? one : zero));
                auto const rF       = static_cast<I>(rT + (adjust ? denom : zero));
                // Said on the counterpart every integer type has, |MIN| fitting in no other.
                assert(xstd::unsigned_abs(rF) < xstd::unsigned_abs(denom));
                assert(xstd::sign(rF) == xstd::sign(denom) or rF == static_cast<I>(0));
                return {.quotient = qF, .remainder = rF};
        }
}

} // namespace xstd

#endif // XSTD_INTS_CSTDLIB_DIV_FLOOR_HPP
