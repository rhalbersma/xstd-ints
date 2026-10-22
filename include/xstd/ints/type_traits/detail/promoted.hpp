//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_TYPE_TRAITS_DETAIL_PROMOTED_HPP
#define XSTD_INTS_TYPE_TRAITS_DETAIL_PROMOTED_HPP

#include <concepts>    // integral
#include <type_traits> // conditional_t, type_identity
#include <utility>     // declval

namespace xstd::ints::detail {

// Held apart because std::conditional forms both type arguments before selecting one.
template<class T>
struct integral_promoted : std::type_identity<decltype(+std::declval<T const&>())>
{};

template<class T>
using promoted = std::conditional_t<std::integral<T>, integral_promoted<T>, std::type_identity<T>>;

} // namespace xstd::ints::detail

#endif // XSTD_INTS_TYPE_TRAITS_DETAIL_PROMOTED_HPP
