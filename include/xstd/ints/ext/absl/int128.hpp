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
#include <type_traits>                             // type_identity

// Abseil introduces the pair and cannot name xstd's traits, so the associations are made here.
namespace xstd {

template<>
struct make_unsigned<absl::int128> : std::type_identity<absl::uint128>
{};

template<>
struct make_signed<absl::uint128> : std::type_identity<absl::int128>
{};

} // namespace xstd

#endif // XSTD_INTS_EXT_ABSL_INT128_HPP
