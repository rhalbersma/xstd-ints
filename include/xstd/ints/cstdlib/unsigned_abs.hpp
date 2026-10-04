//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDLIB_UNSIGNED_ABS_HPP
#define XSTD_INTS_CSTDLIB_UNSIGNED_ABS_HPP

#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/type_traits/is_unsigned.hpp>          // is_unsigned_v
#include <xstd/ints/type_traits/make_unsigned.hpp>        // make_unsigned_t

namespace xstd {

// Like Rust's unsigned_abs: total, returning the unsigned counterpart so MIN is valid.
template<integer I>
[[nodiscard]] constexpr auto unsigned_abs(I x) noexcept(nothrow_const_operators<I>)
        -> make_unsigned_t<I>
{
        if constexpr (is_unsigned_v<I>) {
                return x;
        } else {
                using U         = make_unsigned_t<I>;
                auto const zero = static_cast<U>(0);
                auto const u    = static_cast<U>(x);
                return static_cast<U>(x < static_cast<I>(0) ? zero - u : u);
        }
}

} // namespace xstd

#endif // XSTD_INTS_CSTDLIB_UNSIGNED_ABS_HPP
