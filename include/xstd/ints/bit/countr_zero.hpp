//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_COUNTR_ZERO_HPP
#define XSTD_INTS_BIT_COUNTR_ZERO_HPP

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <bit>
#include <concepts> // unsigned_integral // countr_zero

// See bit/popcount.hpp for why the 128-bit integer classes carry their own overloads.
namespace xstd {

// Both traits, which is exactly <bit>'s domain and no wider. std::unsigned_integral alone would not be:
// it admits bool and char8_t, char16_t and char32_t, which <bit> refuses, so an overload constrained on it
// advertises four types it cannot serve and hard-errors inside instead of not matching. xstd::unsigned_integer
// alone would not be either, in the other direction: it is the open trait, and admits unsigned _BitInt(N) and
// the 128-bit integer classes, which <bit> also refuses. The conjunction is the intersection, and the test
// asserts that equality rather than trusting it.
template<class T>
        requires xstd::unsigned_integer<T> and std::unsigned_integral<T>
[[nodiscard]] constexpr auto countr_zero(T x) noexcept
        -> int
{
        return std::countr_zero(x);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_COUNTR_ZERO_HPP
