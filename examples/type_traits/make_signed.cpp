//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdint/int128.hpp>            // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <concepts>                                // same_as
#include <cstdint>                                 // int8_t, int32_t, int64_t, uint8_t, uint32_t, uint64_t
#include <iostream>                                // cout

static_assert(std::same_as<xstd::make_signed_t<std::uint32_t>, std::int32_t>);
static_assert(std::same_as<xstd::make_signed_t<std::uint8_t const>, std::int8_t const>);
static_assert(std::same_as<xstd::make_signed_t<char>, signed char>);

// bool and double have no member type, which a concept tests for without a hard error.
template<class T>
concept has_signed_counterpart = requires { typename xstd::make_signed_t<T>; };

static_assert(not has_signed_counterpart<bool>);
static_assert(not has_signed_counterpart<double>);

#ifdef XSTD_HAS_INT128
static_assert(std::same_as<xstd::make_signed_t<xstd::uint128>, xstd::int128>);
#endif

// The signed distance from b to a, for unsigned indices whose difference fits the signed half.
template<xstd::unsigned_integer T>
[[nodiscard]] constexpr auto signed_difference(T a, T b) noexcept
        -> xstd::make_signed_t<T>
{
        return static_cast<xstd::make_signed_t<T>>(static_cast<T>(a - b));
}

auto main() -> int
{
        std::cout << static_cast<int>(signed_difference(std::uint8_t{10}, std::uint8_t{3})) << '\n';
        std::cout << static_cast<int>(signed_difference(std::uint8_t{3}, std::uint8_t{10})) << '\n';
        std::cout << signed_difference(std::uint32_t{3}, std::uint32_t{1'000'000}) << '\n';
        std::cout << signed_difference(std::uint64_t{0}, std::uint64_t{1}) << '\n';
}
