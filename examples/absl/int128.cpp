//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/popcount.hpp>              // popcount
#include <xstd/ints/charconv/to_chars.hpp>         // to_chars, to_chars_max_size
#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdlib/div_euclid.hpp>        // div_euclid
#include <xstd/ints/cstdlib/div_floor.hpp>         // div_floor
#include <xstd/ints/cstdlib/unsigned_abs.hpp>      // unsigned_abs
#include <xstd/ints/ext/absl/int128.hpp>           // int128, uint128, make_signed, make_unsigned
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <array>                                   // array
#include <concepts>                                // same_as
#include <iostream>                                // cout
#include <string>                                  // string

static_assert(std::same_as<xstd::make_unsigned_t<absl::int128>, absl::uint128>);
static_assert(std::same_as<xstd::make_signed_t<absl::uint128>, absl::int128>);
static_assert(xstd::signed_integer<absl::int128> and xstd::unsigned_integer<absl::uint128>);

namespace {

template<class I>
[[nodiscard]] auto as_text(I value)
        -> std::string
{
        auto buffer       = std::array<char, xstd::to_chars_max_size<I>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        return {buffer.data(), result.ptr};
}

} // namespace

auto main() -> int
{
        auto const max = xstd::numeric_limits<absl::uint128>::max();
        auto const min = xstd::numeric_limits<absl::int128>::min();
        std::cout << "uint128 max:        " << as_text(max) << '\n';
        std::cout << "popcount(max):      " << xstd::popcount(max) << '\n';
        std::cout << "int128 min:         " << as_text(min) << '\n';
        std::cout << "unsigned_abs(min):  " << as_text(xstd::unsigned_abs(min)) << '\n';

        auto const [floor_q, floor_r]   = xstd::div_floor(absl::int128{-7}, absl::int128{2});
        auto const [euclid_q, euclid_r] = xstd::div_euclid(absl::int128{-7}, absl::int128{-2});
        std::cout << "div_floor(-7, 2):   " << as_text(floor_q) << " remainder " << as_text(floor_r) << '\n';
        std::cout << "div_euclid(-7, -2): " << as_text(euclid_q) << " remainder " << as_text(euclid_r) << '\n';
}
