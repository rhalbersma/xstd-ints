//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_UNSIGNED_ABS_HPP
#define XSTD_INTS_DETAIL_UNSIGNED_ABS_HPP

#include <xstd/ints/detail/integer.hpp>                 // integer
#include <xstd/ints/detail/is_unsigned.hpp>             // is_unsigned_v
#include <xstd/ints/detail/make_unsigned.hpp>           // make_unsigned_t
#include <xstd/ints/detail/nothrow_const_operators.hpp> // nothrow_const_operators

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
                // At zero both arms give zero, so < and <= are the same test.
                return static_cast<U>(x < static_cast<I>(0) ? zero - u : u); // mull-ignore: cxx_lt_to_le
        }
}

} // namespace xstd

#endif // XSTD_INTS_DETAIL_UNSIGNED_ABS_HPP
