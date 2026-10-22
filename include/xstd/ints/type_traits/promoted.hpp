//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_TYPE_TRAITS_PROMOTED_HPP
#define XSTD_INTS_TYPE_TRAITS_PROMOTED_HPP

#include <xstd/ints/type_traits/detail/promoted.hpp> // promoted

namespace xstd {

// The type of +a for an integral T, which [conv.prom] gives; T itself otherwise.
template<class T>
using promoted_t = ints::detail::promoted<T>::type;

} // namespace xstd

#endif // XSTD_INTS_TYPE_TRAITS_PROMOTED_HPP
