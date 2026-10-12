//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDINT_INT128_HPP
#define XSTD_INTS_CSTDINT_INT128_HPP

#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned
#include <type_traits>                             // type_identity

// Prefer the Microsoft STL's integer classes, else the compiler's 128-bit extension; XSTD_HAS_INT128 marks either.
#ifdef _MSC_VER

#define XSTD_HAS_INT128 1

#include <__msvc_int128.hpp>

namespace xstd {

using int128  = std::_Signed128;
using uint128 = std::_Unsigned128;

} // namespace xstd

#elifdef __SIZEOF_INT128__

#define XSTD_HAS_INT128 1

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

namespace xstd {

using int128  = __int128;
using uint128 = unsigned __int128;

} // namespace xstd

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif

// 32-bit x86 and ARM have neither, and there the pair is absent rather than an error.
#ifdef XSTD_HAS_INT128

// The pair's associations, where the pair is introduced: whoever can name it has them.
namespace xstd {

template<>
struct make_unsigned<int128> : std::type_identity<uint128>
{};

template<>
struct make_signed<uint128> : std::type_identity<int128>
{};

} // namespace xstd

#endif

#endif // XSTD_INTS_CSTDINT_INT128_HPP
