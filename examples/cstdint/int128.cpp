//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdint/int128.hpp>            // XSTD_HAS_INT128, int128, uint128
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <concepts>                                // same_as
#include <cstdint>                                 // uint64_t
#include <iomanip>                                 // hex, setfill, setw
#include <iostream>                                // cout

#ifdef XSTD_HAS_INT128
static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128>, xstd::uint128>);
static_assert(std::same_as<xstd::make_signed_t<xstd::uint128>, xstd::int128>);
static_assert(xstd::signed_integer<xstd::int128> and xstd::unsigned_integer<xstd::uint128>);
static_assert(xstd::numeric_limits<xstd::int128>::digits == 127 and xstd::numeric_limits<xstd::uint128>::digits == 128);
static_assert(xstd::int128{-1} < xstd::int128{0});
#endif

namespace {

struct wide_product
{
        std::uint64_t high, low;
};

// The full 128-bit product of two 64-bit factors, in one multiplication where the pair exists.
[[nodiscard]] constexpr auto multiply(std::uint64_t a, std::uint64_t b)
        -> wide_product
{
#ifdef XSTD_HAS_INT128
        auto const product = xstd::uint128{a} * xstd::uint128{b};
        return {.high = static_cast<std::uint64_t>(product >> 64U), .low = static_cast<std::uint64_t>(product)};
#else
        constexpr auto half = std::uint64_t{0xffff'ffff};
        auto const ll       = (a & half) * (b & half);
        auto const lh       = (a & half) * (b >> 32U);
        auto const hl       = (a >> 32U) * (b & half);
        auto const hh       = (a >> 32U) * (b >> 32U);
        auto const middle   = (ll >> 32U) + (lh & half) + (hl & half);
        return {.high = hh + (lh >> 32U) + (hl >> 32U) + (middle >> 32U), .low = (middle << 32U) | (ll & half)};
#endif
}

auto print_product(std::uint64_t a, std::uint64_t b)
        -> void
{
        auto const [high, low] = multiply(a, b);
        std::cout << std::hex << std::setfill('0') << "0x" << std::setw(16) << a << " * 0x" << std::setw(16) << b << " = 0x" << std::setw(16) << high << std::setw(16) << low << '\n';
}

} // namespace

auto main() -> int
{
        print_product(0xffff'ffff'ffff'ffff, 0xffff'ffff'ffff'ffff);
        print_product(0x0123'4567'89ab'cdef, 0xfedc'ba98'7654'3210);
        print_product(0x0000'0001'0000'0000, 0x0000'0001'0000'0000);
}
