//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_POPCOUNT_HPP
#define XSTD_INTS_BIT_POPCOUNT_HPP

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <bit>
#include <concepts> // unsigned_integral // popcount

// The bit basis, one overload set per function. <bit> takes std::unsigned_integral alone, which every
// 128-bit integer CLASS fails -- MSVC's std::_Unsigned128, absl::uint128 and boost::int128::uint128 alike --
// so each of those carries its own overload beside the header that introduces it, reading the words the type
// already holds. GCC's and Clang's unsigned __int128 is not a class and needs nothing: it reaches
// std::unsigned_integral outside __STRICT_ANSI__, which is the mode the bits matrix compiles in.
namespace xstd {

// Both traits, which is exactly <bit>'s domain and no wider. std::unsigned_integral alone would not be:
// it admits bool and char8_t, char16_t and char32_t, which <bit> refuses, so an overload constrained on it
// advertises four types it cannot serve and hard-errors inside instead of not matching. xstd::unsigned_integer
// alone would not be either, in the other direction: it is the open trait, and admits unsigned _BitInt(N) and
// the 128-bit integer classes, which <bit> also refuses. The conjunction is the intersection, and the test
// asserts that equality rather than trusting it.
template<class T>
        requires xstd::unsigned_integer<T> and std::unsigned_integral<T>
[[nodiscard]] constexpr auto popcount(T x) noexcept
        -> int
{
        return std::popcount(x);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_POPCOUNT_HPP
