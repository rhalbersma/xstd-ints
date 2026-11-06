//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_IS_UNSIGNED_HPP
#define XSTD_INTS_DETAIL_IS_UNSIGNED_HPP

#include <xstd/ints/detail/integer_class.hpp> // integer_class
#include <concepts>                           // integral
#include <type_traits>                        // bool_constant, is_unsigned_v, remove_cv_t

namespace xstd {

// std::is_unsigned, which already answers for every type it covers and false for the rest.
template<class T>
inline constexpr auto is_unsigned_v = std::is_unsigned_v<T>;

// Where std stops, asked of the operations alone: -1 lands above zero only where it wrapped.
template<integer_class I_cv>
        requires (not std::integral<I_cv>)
inline constexpr auto is_unsigned_v<I_cv> = [] -> bool {
        using I = std::remove_cv_t<I_cv>;
        return static_cast<I>(0) < static_cast<I>(-1);
}();

template<class T>
using is_unsigned = std::bool_constant<is_unsigned_v<T>>;

} // namespace xstd

#endif // XSTD_INTS_DETAIL_IS_UNSIGNED_HPP
