//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDLIB_ABS_HPP
#define XSTD_INTS_CSTDLIB_ABS_HPP

#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <xstd/ints/type_traits/is_unsigned.hpp>          // is_unsigned_v
#include <cassert>                                        // assert

namespace xstd {

// constexpr generalization of abs/labs/llabs/imaxabs.
template<integer I>
[[nodiscard]] constexpr auto abs(I x) noexcept(nothrow_const_operators<I>)
        -> I
{
        // An unsigned value is its own magnitude, and its min() is 0.
        if constexpr (is_unsigned_v<I>) {
                return x;
        } else {
                auto const zero = static_cast<I>(0);
                assert(x != numeric_limits<I>::min()); // -x would overflow
                // At zero both arms give zero, so < and <= are the same test.
                return static_cast<I>(x < zero ? -x : x); // mull-ignore: cxx_lt_to_le
        }
}

} // namespace xstd

#endif // XSTD_INTS_CSTDLIB_ABS_HPP
