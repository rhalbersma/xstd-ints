//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_EXT_BOOST_INT128_HPP
#define XSTD_INTS_EXT_BOOST_INT128_HPP

// IWYU pragma: always_keep

#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned
#include <boost/int128.hpp>                        // IWYU pragma: export; int128, uint128
#include <bit>                                     // countl_zero, countr_zero, popcount
#include <limits>                                  // numeric_limits
#include <type_traits>                             // type_identity

// The export stops where Boost's own do: its pair is declared in detail/, which no public header re-exports.

// Boost.Int128 introduces the pair and cannot name xstd's traits, so the associations are made here.
namespace xstd {

// Spelled without the _t suffix upstream dropped after its last release tag.
template<>
struct make_unsigned<boost::int128::int128> : std::type_identity<boost::int128::uint128>
{};

template<>
struct make_signed<boost::int128::uint128> : std::type_identity<boost::int128::int128>
{};

// boost::int128::uint128 is a class, so <bit> declines it; read by NAME, upstream reversing the halves' declaration order by endianness. [xstd/ints/bit/popcount.hpp]
[[nodiscard]] constexpr auto popcount(boost::int128::uint128 x) noexcept
        -> int
{
        return std::popcount(x.low) + std::popcount(x.high);
}

[[nodiscard]] constexpr auto countl_zero(boost::int128::uint128 x) noexcept
        -> int
{
        return x.high != 0 ? std::countl_zero(x.high)
                           : std::numeric_limits<decltype(x.low)>::digits + std::countl_zero(x.low);
}

[[nodiscard]] constexpr auto countr_zero(boost::int128::uint128 x) noexcept
        -> int
{
        return x.low != 0 ? std::countr_zero(x.low)
                          : std::numeric_limits<decltype(x.high)>::digits + std::countr_zero(x.high);
}

} // namespace xstd

#endif // XSTD_INTS_EXT_BOOST_INT128_HPP
