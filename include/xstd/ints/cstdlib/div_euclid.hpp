//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDLIB_DIV_EUCLID_HPP
#define XSTD_INTS_CSTDLIB_DIV_EUCLID_HPP

#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdlib/div.hpp>                      // div
#include <xstd/ints/cstdlib/div_result.hpp>               // IWYU pragma: export; div_result
#include <xstd/ints/cstdlib/sign.hpp>                     // sign
#include <xstd/ints/cstdlib/unsigned_abs.hpp>             // unsigned_abs
#include <xstd/ints/type_traits/is_unsigned.hpp>          // is_unsigned_v
#include <cassert>                                        // assert

namespace xstd {

// Euclidean division: the remainder is nonnegative.
template<integer I>
[[nodiscard]] constexpr auto div_euclid(I numer, I denom) noexcept(nothrow_const_operators<I>)
        -> div_result<I>
{
        assert(denom != static_cast<I>(0));
        // An unsigned truncated remainder is already nonnegative, so it is the answer.
        if constexpr (is_unsigned_v<I>) {
                // Qualified: unqualified, ADL finds Boost.Int128's own div and it wins.
                return xstd::div(numer, denom);
        } else {
                auto const [qT, rT] = xstd::div(numer, denom);
                auto const zero     = static_cast<I>(0);
                auto const one      = static_cast<I>(1);
                auto const adjust   = rT < zero;
                auto const qE       = adjust ? (denom > zero ? static_cast<I>(qT - one) : static_cast<I>(qT + one)) : qT;
                auto const rE       = adjust ? (denom > zero ? static_cast<I>(rT + denom) : static_cast<I>(rT - denom)) : rT;
                // Said on the counterpart every integer type has, |MIN| fitting in no other.
                assert(xstd::unsigned_abs(rE) < xstd::unsigned_abs(denom));
                assert(xstd::sign(rE) >= 0);
                return {.quotient = qE, .remainder = rE};
        }
}

} // namespace xstd

#endif // XSTD_INTS_CSTDLIB_DIV_EUCLID_HPP
