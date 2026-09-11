//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_TYPE_TRAITS_IS_CHARACTER_HPP
#define XSTD_INTS_TYPE_TRAITS_IS_CHARACTER_HPP

#include <type_traits> // bool_constant, is_same_v, remove_cv_t

namespace xstd {

// [basic.fundamental]'s five character types, cv-transparent as std's own traits are. Closed where xstd's
// signedness traits are open: the core language draws this list and no user type can join it, so there is
// nothing to extend. Note that signed char and unsigned char are NOT here -- they are narrow character types
// but standard integer types, which is what keeps int8_t and uint8_t out of this trait's answer.
template<class T>
inline constexpr auto is_character_v =
        std::is_same_v<std::remove_cv_t<T>, char> or
        std::is_same_v<std::remove_cv_t<T>, wchar_t> or
        std::is_same_v<std::remove_cv_t<T>, char8_t> or
        std::is_same_v<std::remove_cv_t<T>, char16_t> or
        std::is_same_v<std::remove_cv_t<T>, char32_t>;

template<class T>
using is_character = std::bool_constant<is_character_v<T>>;

} // namespace xstd

#endif // XSTD_INTS_TYPE_TRAITS_IS_CHARACTER_HPP
