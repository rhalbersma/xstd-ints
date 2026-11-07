//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdint/bit_int.hpp>           // XSTD_HAS_BIT_INT, bit_int, bit_int_max_width, bit_uint
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <concepts>                                // same_as
#include <cstdint>                                 // uint32_t
#include <iomanip>                                 // hex, setfill, setw
#include <iostream>                                // cout

#ifdef XSTD_HAS_BIT_INT
static_assert(xstd::bit_int_max_width >= 128);
static_assert(std::same_as<xstd::make_signed_t<xstd::bit_uint<24>>, xstd::bit_int<24>>);
static_assert(xstd::signed_integer<xstd::bit_int<24>> and xstd::unsigned_integer<xstd::bit_uint<24>>);
static_assert(xstd::numeric_limits<xstd::bit_int<24>>::digits == 23 and xstd::numeric_limits<xstd::bit_uint<24>>::digits == 24);
static_assert(xstd::numeric_limits<xstd::bit_uint<24>>::max() == 0xff'ffff);
#endif

namespace {

// Addition modulo 2^24: a bit-precise type wraps at its own width, and without one a mask does.
[[nodiscard]] constexpr auto add_wrapping_24(std::uint32_t a, std::uint32_t b)
        -> std::uint32_t
{
#ifdef XSTD_HAS_BIT_INT
        using u24 = xstd::bit_uint<24>;
        return static_cast<std::uint32_t>(static_cast<u24>(a) + static_cast<u24>(b));
#else
        return (a + b) & 0xff'ffffU;
#endif
}

auto print_sum(std::uint32_t a, std::uint32_t b)
        -> void
{
        std::cout << std::hex << std::setfill('0') << "0x" << std::setw(6) << a << " + 0x" << std::setw(6) << b << " = 0x" << std::setw(6) << add_wrapping_24(a, b) << '\n';
}

} // namespace

auto main() -> int
{
        print_sum(0x12'3456, 0x11'1111);
        print_sum(0xff'fff0, 0x00'0020);
        print_sum(0xff'ffff, 0xff'ffff);
}
