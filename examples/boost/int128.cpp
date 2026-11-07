//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_width.hpp>             // bit_width
#include <xstd/ints/bit/countl_zero.hpp>           // countl_zero
#include <xstd/ints/bit/has_single_bit.hpp>        // has_single_bit
#include <xstd/ints/charconv/to_chars.hpp>         // to_chars, to_chars_max_size
#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdlib/abs.hpp>               // abs
#include <xstd/ints/cstdlib/div_floor.hpp>         // div_floor
#include <xstd/ints/cstdlib/div_result.hpp>        // div_result
#include <xstd/ints/cstdlib/sign.hpp>              // sign
#include <xstd/ints/ext/boost/int128.hpp>          // int128, uint128, make_signed, make_unsigned
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <array>                                   // array
#include <concepts>                                // same_as
#include <iostream>                                // boolalpha, cout
#include <string>                                  // string

using boost::int128::int128;
using boost::int128::uint128;

static_assert(std::same_as<xstd::make_unsigned_t<int128>, uint128>);
static_assert(std::same_as<xstd::make_signed_t<uint128>, int128>);
static_assert(xstd::signed_integer<int128> and xstd::unsigned_integer<uint128>);
static_assert(xstd::div_floor(int128{-7}, int128{2}) == xstd::div_result{int128{-4}, int128{1}});

namespace {

template<class I>
[[nodiscard]] auto as_text(I value, int base = 10)
        -> std::string
{
        auto buffer       = std::array<char, xstd::to_chars_max_size<I>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        return {buffer.data(), result.ptr};
}

} // namespace

auto main() -> int
{
        auto const power    = uint128{1} << 100U;
        auto const negative = -static_cast<int128>(power);
        std::cout << std::boolalpha;
        std::cout << "2^100:                 " << as_text(power) << '\n';
        std::cout << "2^100 in base 16:      " << as_text(power, 16) << '\n';
        std::cout << "has_single_bit(2^100): " << xstd::has_single_bit(power) << '\n';
        std::cout << "bit_width(2^100):      " << xstd::bit_width(power) << '\n';
        std::cout << "countl_zero(2^100):    " << xstd::countl_zero(power) << '\n';
        std::cout << "sign(-2^100):          " << xstd::sign(negative) << '\n';
        std::cout << "abs(-2^100):           " << as_text(xstd::abs(negative)) << '\n';
}
