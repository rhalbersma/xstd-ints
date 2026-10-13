//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_SHIFT_COUNT_HPP
#define XSTD_INTS_BIT_DETAIL_SHIFT_COUNT_HPP

#include <type_traits> // conditional_t, is_class_v

namespace xstd::ints::detail {

// Unsigned for a builtin shift, so no count is signed; int for a class type's, as absl::uint128's operator<< takes.
template<class T>
using shift_count_t = std::conditional_t<std::is_class_v<T>, int, unsigned>;

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_SHIFT_COUNT_HPP
