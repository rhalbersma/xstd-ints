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
#include <xstd/ints/type_traits/promoted.hpp>             // promoted_t
#include <cassert>                                        // assert

namespace xstd {

// constexpr generalization of abs/labs/llabs/imaxabs, in the type std::abs gives: a type narrower than int becomes int.
template<integer I>
[[nodiscard]] constexpr auto abs(I x) noexcept(nothrow_const_operators<I>)
        -> promoted_t<I>
{
        using P = promoted_t<I>;
        // An unsigned value is its own magnitude, and its min() is 0.
        if constexpr (is_unsigned_v<P>) {
                return +x;
        } else {
                auto const p    = +x;
                auto const zero = static_cast<P>(0);
                assert(p != numeric_limits<P>::min()); // -p would overflow
                // At zero both arms give zero, so < and <= are the same test.
                return static_cast<P>(p < zero ? -p : p); // mull-ignore: cxx_lt_to_le
        }
}

} // namespace xstd

#endif // XSTD_INTS_CSTDLIB_ABS_HPP
