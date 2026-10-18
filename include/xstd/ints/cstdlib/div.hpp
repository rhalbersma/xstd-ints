//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDLIB_DIV_HPP
#define XSTD_INTS_CSTDLIB_DIV_HPP

#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdlib/div_result.hpp>               // IWYU pragma: export; div_result
#include <xstd/ints/cstdlib/sign.hpp>                     // sign
#include <xstd/ints/cstdlib/unsigned_abs.hpp>             // unsigned_abs
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp>            // is_signed_v
#include <cassert>                                        // assert

namespace xstd {

// Truncated division, as specified by [expr.mul].
template<integer I>
[[nodiscard]] constexpr auto div(I numer, I denom) noexcept(nothrow_const_operators<I>)
        -> div_result<I>
{
        assert(denom != static_cast<I>(0));
        // Only a signed type has a MIN to reach it with; unsigned, this refuses div(0, max).
        if constexpr (is_signed_v<I>) {
                assert(numer != numeric_limits<I>::min() or denom != static_cast<I>(-1));
        }
        // Prvalue operands: x86 MSVC's optimizer lets the 128-bit STL division write through to named ones.
        // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
        auto const& qT = static_cast<I>(I{numer} / I{denom});
        auto const& rT = static_cast<I>(I{numer} % I{denom});
        assert(numer == static_cast<I>(static_cast<I>(denom * qT) + rT));
        // Said on the counterpart every integer type has, |MIN| fitting in no other.
        assert(xstd::unsigned_abs(rT) < xstd::unsigned_abs(denom));
        assert(xstd::sign(rT) == xstd::sign(numer) or rT == static_cast<I>(0));
        return {.quotient = qT, .remainder = rT};
}

} // namespace xstd

#endif // XSTD_INTS_CSTDLIB_DIV_HPP
