//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer.hpp>          // integer
#include <xstd/ints/cstdint/int128.hpp>            // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <cstdint>                                 // int8_t, int32_t, uint8_t, uint64_t
#include <iostream>                                // cout
#include <limits>                                  // numeric_limits

static_assert(xstd::integer<signed char>);
static_assert(xstd::integer<std::uint8_t const>);
static_assert(xstd::integer<std::int32_t>);

// Unlike std::integral, the character types and bool are no integers.
static_assert(not xstd::integer<char>);
static_assert(not xstd::integer<char8_t>);
static_assert(not xstd::integer<bool>);

#ifdef XSTD_HAS_INT128
static_assert(xstd::integer<xstd::int128>);
static_assert(xstd::integer<xstd::uint128>);
#endif

// The distance between two integers, which always fits in the unsigned half of the pair.
template<xstd::integer T>
[[nodiscard]] constexpr auto abs_diff(T a, T b) noexcept
        -> xstd::make_unsigned_t<T>
{
        using U = xstd::make_unsigned_t<T>;
        return a < b ? static_cast<U>(static_cast<U>(b) - static_cast<U>(a)) : static_cast<U>(static_cast<U>(a) - static_cast<U>(b));
}

auto main() -> int
{
        using i8  = std::numeric_limits<std::int8_t>;
        using i32 = std::numeric_limits<std::int32_t>;
        std::cout << static_cast<unsigned>(abs_diff(i8::min(), i8::max())) << '\n';
        std::cout << abs_diff(i32::max(), i32::min()) << '\n';
        std::cout << abs_diff(std::uint64_t{7}, std::uint64_t{19}) << '\n';
}
