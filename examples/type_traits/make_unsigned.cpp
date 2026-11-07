//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/cstdint/int128.hpp>            // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <concepts>                                // same_as
#include <cstdint>                                 // int8_t, int32_t, int64_t, uint8_t, uint32_t
#include <iostream>                                // cout
#include <limits>                                  // numeric_limits

static_assert(std::same_as<xstd::make_unsigned_t<std::int32_t>, std::uint32_t>);
static_assert(std::same_as<xstd::make_unsigned_t<std::int8_t const volatile>, std::uint8_t const volatile>);
static_assert(std::same_as<xstd::make_unsigned_t<char>, unsigned char>);

// bool and int* have no member type, which a concept tests for where std::make_unsigned_t is a hard error.
template<class T>
concept has_unsigned_counterpart = requires { typename xstd::make_unsigned_t<T>; };

static_assert(not has_unsigned_counterpart<bool>);
static_assert(not has_unsigned_counterpart<int*>);

#ifdef XSTD_HAS_INT128
static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128>, xstd::uint128>);
#endif

// The absolute value of a signed integer, in the unsigned half of its pair, where even the minimum fits.
template<xstd::signed_integer T>
[[nodiscard]] constexpr auto magnitude(T value) noexcept
        -> xstd::make_unsigned_t<T>
{
        using U = xstd::make_unsigned_t<T>;
        return value < static_cast<T>(0) ? static_cast<U>(static_cast<U>(0) - static_cast<U>(value)) : static_cast<U>(value);
}

auto main() -> int
{
        std::cout << static_cast<unsigned>(magnitude(std::numeric_limits<std::int8_t>::min())) << '\n';
        std::cout << magnitude(std::numeric_limits<std::int32_t>::min()) << '\n';
        std::cout << magnitude(std::int64_t{-5}) << '\n';
}
