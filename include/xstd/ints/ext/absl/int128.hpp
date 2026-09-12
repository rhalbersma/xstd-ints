//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_EXT_ABSL_INT128_HPP
#define XSTD_INTS_EXT_ABSL_INT128_HPP

// IWYU pragma: always_keep

#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned
#include <absl/numeric/int128.h>                   // IWYU pragma: export; int128, uint128
#include <bit>                                     // countl_zero, countr_zero, popcount
#include <limits>                                  // numeric_limits
#include <type_traits>                             // type_identity

// Abseil introduces the pair and cannot name xstd's traits, so the associations are made here.
namespace xstd {

template<>
struct make_unsigned<absl::int128> : std::type_identity<absl::uint128>
{};

template<>
struct make_signed<absl::uint128> : std::type_identity<absl::int128>
{};

// absl::uint128 is a class, so <bit> declines it; these read its own words through Abseil's documented accessors, found by ADL. [xstd/ints/bit/popcount.hpp]
[[nodiscard]] constexpr auto popcount(absl::uint128 x) noexcept
        -> int
{
        return std::popcount(Uint128Low64(x)) + std::popcount(Uint128High64(x));
}

[[nodiscard]] constexpr auto countl_zero(absl::uint128 x) noexcept
        -> int
{
        return Uint128High64(x) != 0
                       ? std::countl_zero(Uint128High64(x))
                       : std::numeric_limits<decltype(Uint128Low64(x))>::digits + std::countl_zero(Uint128Low64(x));
}

[[nodiscard]] constexpr auto countr_zero(absl::uint128 x) noexcept
        -> int
{
        return Uint128Low64(x) != 0
                       ? std::countr_zero(Uint128Low64(x))
                       : std::numeric_limits<decltype(Uint128Low64(x))>::digits + std::countr_zero(Uint128High64(x));
}

} // namespace xstd

#endif // XSTD_INTS_EXT_ABSL_INT128_HPP
