//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/charconv/to_chars.hpp>     // to_chars, to_chars_max_size
#include <xstd/ints/concepts/integer.hpp>      // integer
#include <xstd/ints/cstdint/int128.hpp>        // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <array>                               // array
#include <cstdint>                             // int8_t, int32_t, uint64_t
#include <ios>                                 // boolalpha
#include <iostream>                            // cout
#include <string>                              // string
#include <system_error>                        // errc

// A buffer of to_chars_max_size<I> characters holds every value of I in every base, base 2 being the longest.
template<xstd::integer I>
[[nodiscard]] constexpr auto to_digits(I value, int base = 10)
        -> std::string
{
        auto buffer       = std::array<char, xstd::to_chars_max_size<I>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        return {buffer.data(), result.ptr};
}

static_assert(xstd::to_chars_max_size<std::int8_t> == 9);
static_assert(to_digits(std::int8_t{-128}, 2) == "-10000000");

#ifdef XSTD_HAS_INT128
// A 128-bit integer is written as well, at compile time too.
static_assert(xstd::to_chars_max_size<xstd::uint128> == 128);
static_assert(to_digits(xstd::numeric_limits<xstd::int128>::min()) == "-170141183460469231731687303715884105728");
static_assert(to_digits(xstd::numeric_limits<xstd::uint128>::max(), 16) == "ffffffffffffffffffffffffffffffff");
#endif

auto main() -> int
{
        std::cout << to_digits(std::int32_t{-255}) << '\n';
        std::cout << to_digits(std::int32_t{-255}, 16) << '\n';
        std::cout << to_digits(std::int32_t{-255}, 2) << '\n';
        std::cout << to_digits(xstd::numeric_limits<std::uint64_t>::max(), 36) << '\n';

        // A buffer too short for the digits reports it, rather than writing past its end.
        auto small        = std::array<char, 3>{};
        auto const result = xstd::to_chars(small.data(), small.data() + small.size(), std::int32_t{1000});
        std::cout << std::boolalpha << (result.ec == std::errc::value_too_large) << '\n';
}
