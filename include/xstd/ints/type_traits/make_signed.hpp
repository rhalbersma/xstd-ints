//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_TYPE_TRAITS_MAKE_SIGNED_HPP
#define XSTD_INTS_TYPE_TRAITS_MAKE_SIGNED_HPP

#include <xstd/ints/concepts/integer_class.hpp> // integer_class
#include <xstd/ints/type_traits/is_signed.hpp>  // is_signed_v
#include <concepts>                             // integral, same_as
#include <cstddef>                              // size_t
#include <type_traits>                          // add_const_t, add_cv_t, add_volatile_t, is_enum_v, make_signed, remove_cv_t, type_identity

namespace xstd {

// Signed counterparts; the empty primary makes an unsupported association a failure.
template<class T>
struct make_signed
{};

template<class T>
using make_signed_t = make_signed<T>::type; // none for an integer-class type until make_signed is specialized for it, as its adaptor does

// [meta.trans.sign]/2's mandated domain, less bool; cv goes through the specializations below, as _BitInt(1) must.
template<class T>
        requires (std::integral<T> or std::is_enum_v<T>) and std::same_as<T, std::remove_cv_t<T>> and (not std::same_as<T, bool>)
struct make_signed<T> : std::make_signed<T>
{};

// Where std stops: its own counterpart, the other half of the pair being the user's to say.
template<integer_class I>
        requires (not std::integral<I>) and is_signed_v<I>
struct make_signed<I> : std::type_identity<I>
{};

template<class T>
        requires requires { typename make_signed_t<T>; }
struct make_signed<T const> : std::type_identity<std::add_const_t<make_signed_t<T>>>
{};

template<class T>
        requires requires { typename make_signed_t<T>; }
struct make_signed<T volatile> : std::type_identity<std::add_volatile_t<make_signed_t<T>>>
{};

template<class T>
        requires requires { typename make_signed_t<T>; }
struct make_signed<T const volatile> : std::type_identity<std::add_cv_t<make_signed_t<T>>>
{};

#ifdef __BITINT_MAXWIDTH__

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbit-int-extension"
#endif

// signed _BitInt(1) is ill-formed until Clang implements WG14 N3747, so unsigned _BitInt(1) has no counterpart.
template<>
struct make_signed<unsigned _BitInt(1)>
{};

template<std::size_t N>
struct make_signed<unsigned _BitInt(N)> : std::type_identity<signed _BitInt(N)>
{};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // __BITINT_MAXWIDTH__

} // namespace xstd

#endif // XSTD_INTS_TYPE_TRAITS_MAKE_SIGNED_HPP
