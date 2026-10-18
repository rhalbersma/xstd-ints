//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_TYPE_TRAITS_MAKE_UNSIGNED_HPP
#define XSTD_INTS_TYPE_TRAITS_MAKE_UNSIGNED_HPP

#include <xstd/ints/concepts/integer_class.hpp>  // integer_class
#include <xstd/ints/type_traits/is_unsigned.hpp> // is_unsigned_v
#include <concepts>                              // integral, same_as
#include <type_traits>                           // add_const_t, add_cv_t, add_volatile_t, is_enum_v, make_unsigned, remove_cv_t, type_identity

namespace xstd {

// Unsigned counterparts; the empty primary makes an unsupported association a failure.
template<class T>
struct make_unsigned
{};

template<class T>
using make_unsigned_t = make_unsigned<T>::type; // none for an integer-class type until make_unsigned is specialized for it, as its adaptor does

// [meta.trans.sign]/2's whole mandated domain, less the cv bool std declines to pair there.
template<class T>
        requires (std::integral<T> or std::is_enum_v<T>) and (not std::same_as<std::remove_cv_t<T>, bool>)
struct make_unsigned<T> : std::make_unsigned<T>
{};

// Where std stops: its own counterpart, the other half of the pair being the user's to say.
template<integer_class I>
        requires (not std::integral<I>) and is_unsigned_v<I>
struct make_unsigned<I> : std::type_identity<I>
{};

template<class T>
        requires requires { typename make_unsigned_t<T>; }
struct make_unsigned<T const> : std::type_identity<std::add_const_t<make_unsigned_t<T>>>
{};

template<class T>
        requires requires { typename make_unsigned_t<T>; }
struct make_unsigned<T volatile> : std::type_identity<std::add_volatile_t<make_unsigned_t<T>>>
{};

template<class T>
        requires requires { typename make_unsigned_t<T>; }
struct make_unsigned<T const volatile> : std::type_identity<std::add_cv_t<make_unsigned_t<T>>>
{};

} // namespace xstd

#endif // XSTD_INTS_TYPE_TRAITS_MAKE_UNSIGNED_HPP
