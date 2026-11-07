//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>       // uint128, XSTD_HAS_INT128
#include <xstd/ints/type_traits/promoted.hpp> // promoted_t
#include <array>                              // array
#include <concepts>                           // integral, same_as
#include <cstddef>                            // byte, size_t
#include <cstdint>                            // int16_t, uint8_t, uint16_t, uint64_t
#include <iostream>                           // cout

static_assert(std::same_as<xstd::promoted_t<std::uint8_t>, int>);
static_assert(std::same_as<xstd::promoted_t<std::int16_t const>, int>);
static_assert(std::same_as<xstd::promoted_t<bool>, int>);
static_assert(std::same_as<xstd::promoted_t<std::uint64_t>, std::uint64_t>);
static_assert(std::same_as<xstd::promoted_t<std::byte>, std::byte>);

#ifdef XSTD_HAS_INT128
static_assert(std::same_as<xstd::promoted_t<xstd::uint128>, xstd::uint128>);
#endif

// The sum of narrow values in the type their arithmetic is done in, so it does not wrap at the element's width.
template<std::integral T, std::size_t N>
[[nodiscard]] constexpr auto total(std::array<T, N> const& values) noexcept
        -> xstd::promoted_t<T>
{
        auto sum = xstd::promoted_t<T>{};
        for (auto const value : values) {
                sum += value;
        }
        return sum;
}

auto main() -> int
{
        constexpr auto narrow = std::uint8_t{0x0F};
        static_assert(std::same_as<decltype(~narrow), xstd::promoted_t<std::uint8_t>>);
        std::cout << ~narrow << '\n';
        std::cout << static_cast<int>(static_cast<std::uint8_t>(~narrow)) << '\n';

        std::cout << total(std::array<std::uint8_t, 3>{200, 100, 50}) << '\n';
        std::cout << total(std::array<std::uint16_t, 2>{60'000, 50'000}) << '\n';
}
