//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer.hpp>          // integer
#include <xstd/ints/cstdint/int128.hpp>            // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/type_traits/is_unsigned.hpp>   // is_unsigned, is_unsigned_v
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <cstdint>                                 // int8_t, int32_t, uint8_t, uint32_t
#include <iostream>                                // cout
#include <limits>                                  // numeric_limits
#include <type_traits>                             // is_unsigned_v

static_assert(xstd::is_unsigned_v<int> == std::is_unsigned_v<int>);
static_assert(xstd::is_unsigned_v<unsigned> == std::is_unsigned_v<unsigned>);
static_assert(xstd::is_unsigned_v<double> == std::is_unsigned_v<double>);
static_assert(xstd::is_unsigned_v<std::uint8_t const>);
static_assert(xstd::is_unsigned<std::uint32_t>::value);

// A class type under MSVC, for which std::is_unsigned_v is false.
#ifdef XSTD_HAS_INT128
static_assert(xstd::is_unsigned_v<xstd::uint128>);
static_assert(not xstd::is_unsigned_v<xstd::int128>);
#endif

// Addition modulo 2^N, which an unsigned type gives and a signed one only through its unsigned counterpart.
template<xstd::integer T>
[[nodiscard]] constexpr auto wrapping_add(T a, T b) noexcept
        -> T
{
        if constexpr (xstd::is_unsigned_v<T>) {
                return static_cast<T>(a + b);
        } else {
                using U = xstd::make_unsigned_t<T>;
                return static_cast<T>(static_cast<U>(static_cast<U>(a) + static_cast<U>(b)));
        }
}

auto main() -> int
{
        std::cout << static_cast<int>(wrapping_add(std::uint8_t{200}, std::uint8_t{100})) << '\n';
        std::cout << static_cast<int>(wrapping_add(std::int8_t{100}, std::int8_t{100})) << '\n';
        std::cout << wrapping_add(std::numeric_limits<std::int32_t>::max(), std::int32_t{1}) << '\n';
}
