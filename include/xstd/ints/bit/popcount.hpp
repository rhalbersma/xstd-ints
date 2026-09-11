//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_POPCOUNT_HPP
#define XSTD_INTS_BIT_POPCOUNT_HPP

#include <bit> // popcount

// The bit basis, one overload set per function. <bit> takes std::unsigned_integral alone, which every
// 128-bit integer CLASS fails -- MSVC's std::_Unsigned128, absl::uint128 and boost::int128::uint128 alike --
// so each of those carries its own overload beside the header that introduces it, reading the words the type
// already holds. GCC's and Clang's unsigned __int128 is not a class and needs nothing: it reaches
// std::unsigned_integral outside __STRICT_ANSI__, which is the mode the bits matrix compiles in.
namespace xstd {

// Constrained on the call itself, so the constraint and the body are the same thing and cannot drift apart.
// Two spellings were tried and measured first. std::unsigned_integral is too wide: it admits bool, char8_t,
// char16_t and char32_t, which <bit> refuses, so an overload constrained on it advertises four types it
// cannot serve and hard-errors inside instead of not matching. Adding xstd::unsigned_integer to it removes
// those four and looks like the exact intersection -- on libstdc++. It is NOT on libc++, which already
// carries unsigned _BitInt(N) into std::unsigned_integral while <bit> still declines it, so the conjunction
// goes true there with the body still ill-formed. That is the split P3666R4 asks for, is_integral true and
// library support "basically nowhere", and one implementation has it already.
//
// Detection is exact on both, and stays exact whichever way that paper lands: unsigned __int128 is in
// outside __STRICT_ANSI__, _BitInt is in if and when <bit> takes it, with nothing to change here. This
// matters because std's traits are closed to us -- we cannot teach them, only ask them.
template<class T>
        requires requires (T x) { std::popcount(x); }
[[nodiscard]] constexpr auto popcount(T x) noexcept
        -> int
{
        return std::popcount(x);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_POPCOUNT_HPP
