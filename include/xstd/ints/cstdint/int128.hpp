//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDINT_INT128_HPP
#define XSTD_INTS_CSTDINT_INT128_HPP

#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned
#include <type_traits>                             // type_identity

// Prefer the Microsoft STL's integer classes, else the compiler's 128-bit extension.
#ifdef _MSC_VER

#include <__msvc_int128.hpp>
#include <bit> // countl_zero, countr_zero, popcount

namespace xstd {

using int128 = std::_Signed128;
using uint128 = std::_Unsigned128;

// std::_Unsigned128 is a class, so <bit> declines it in every mode; these read the words it already holds.
// _Word is unspecified, as the type itself is: naming the alias above already depends on this header, so the
// members add no exposure the alias did not. _Word[0] is the low half -- _Base128::_Left_shift moves _Word[0]
// into _Word[1] past 64 -- and the test pins that rather than trusting the reading.
[[nodiscard]] constexpr auto popcount(uint128 x) noexcept
        -> int
{
        return std::popcount(x._Word[0]) + std::popcount(x._Word[1]);
}

[[nodiscard]] constexpr auto countl_zero(uint128 x) noexcept
        -> int
{
        return x._Word[1] != 0 ? std::countl_zero(x._Word[1]) : 64 + std::countl_zero(x._Word[0]);
}

[[nodiscard]] constexpr auto countr_zero(uint128 x) noexcept
        -> int
{
        return x._Word[0] != 0 ? std::countr_zero(x._Word[0]) : 64 + std::countr_zero(x._Word[1]);
}

} // namespace xstd

#elifdef __SIZEOF_INT128__

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

namespace xstd {

using int128 = __int128;
using uint128 = unsigned __int128;

} // namespace xstd

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#else

#error "xstd::int128 requires GCC/Clang __int128 or the Microsoft STL 128-bit integer classes"

#endif

// The pair's associations, where the pair is introduced: whoever can name it has them.
namespace xstd {

template<>
struct make_unsigned<int128> : std::type_identity<uint128>
{};

template<>
struct make_signed<uint128> : std::type_identity<int128>
{};

} // namespace xstd

#endif // XSTD_INTS_CSTDINT_INT128_HPP
