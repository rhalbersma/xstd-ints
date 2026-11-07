//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer.hpp>      // integer
#include <xstd/ints/cstdint/int128.hpp>        // int128, uint128, XSTD_HAS_INT128
#include <xstd/ints/type_traits/is_signed.hpp> // is_signed, is_signed_v
#include <cstdint>                             // int8_t, int16_t, int32_t, uint16_t, uint64_t
#include <iostream>                            // cout
#include <limits>                              // numeric_limits
#include <string>                              // string, to_string
#include <type_traits>                         // is_signed_v

static_assert(xstd::is_signed_v<int> == std::is_signed_v<int>);
static_assert(xstd::is_signed_v<unsigned> == std::is_signed_v<unsigned>);
static_assert(xstd::is_signed_v<double> == std::is_signed_v<double>);
static_assert(xstd::is_signed_v<std::int8_t const>);
static_assert(xstd::is_signed<std::int16_t>::value);

// A class type under MSVC, for which std::is_signed_v is false.
#ifdef XSTD_HAS_INT128
static_assert(xstd::is_signed_v<xstd::int128>);
static_assert(not xstd::is_signed_v<xstd::uint128>);
#endif

// The exact-width name of an integer type, from its signedness and its width in bits.
template<xstd::integer T>
[[nodiscard]] auto exact_width_name()
        -> std::string
{
        constexpr auto width = std::numeric_limits<T>::digits + (xstd::is_signed_v<T> ? 1 : 0);
        return (xstd::is_signed_v<T> ? "int" : "uint") + std::to_string(width) + "_t";
}

auto main() -> int
{
        std::cout << exact_width_name<std::int8_t>() << '\n';
        std::cout << exact_width_name<std::uint16_t>() << '\n';
        std::cout << exact_width_name<std::int32_t>() << '\n';
        std::cout << exact_width_name<std::uint64_t>() << '\n';
}
