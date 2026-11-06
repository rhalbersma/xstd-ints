//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_SIGN_HPP
#define XSTD_INTS_DETAIL_SIGN_HPP

#include <xstd/ints/detail/integer.hpp>                 // integer
#include <xstd/ints/detail/is_unsigned.hpp>             // is_unsigned_v
#include <xstd/ints/detail/nothrow_const_operators.hpp> // nothrow_const_operators

namespace xstd {

// Returns -1, 0, or 1; over an unsigned type, 0 or 1.
template<integer I>
[[nodiscard]] constexpr auto sign(I x) noexcept(nothrow_const_operators<I>)
        -> int
{
        auto const zero = static_cast<I>(0);
        // One comparison, not two: for an integer-class type the dead one is a call.
        if constexpr (is_unsigned_v<I>) {
                return static_cast<int>(zero < x);
        } else {
                return static_cast<int>(zero < x) - static_cast<int>(x < zero);
        }
}

} // namespace xstd

#endif // XSTD_INTS_DETAIL_SIGN_HPP
