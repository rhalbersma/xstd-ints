//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_SIGNED_INTEGER_HPP
#define XSTD_INTS_DETAIL_SIGNED_INTEGER_HPP

#include <xstd/ints/detail/integer.hpp>   // integer
#include <xstd/ints/detail/is_signed.hpp> // is_signed_v

namespace xstd {

// Mirrors std::signed_integral, keeping integer atomic for subsumption.
template<class T>
concept signed_integer = integer<T> and is_signed_v<T>;

} // namespace xstd

#endif // XSTD_INTS_DETAIL_SIGNED_INTEGER_HPP
