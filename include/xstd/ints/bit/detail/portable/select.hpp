//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_PORTABLE_SELECT_HPP
#define XSTD_INTS_BIT_DETAIL_PORTABLE_SELECT_HPP

#include <xstd/ints/bit/detail/bit_limb.hpp> // bit_limb
#include <array>                             // array
#include <bit>                               // popcount
#include <cassert>                           // assert
#include <cstddef>                           // size_t
#include <cstdint>                           // uint8_t
#include <limits>                            // numeric_limits
#include <ranges>                            // iota
#include <utility>                           // cmp_less

namespace xstd::ints::detail::portable {

inline constexpr auto byte_digits = static_cast<unsigned>(std::numeric_limits<std::uint8_t>::digits);
inline constexpr auto byte_values = 1UZ << byte_digits;

// The position of the k-th one of each byte value, at index byte | (k << byte_digits).
inline constexpr auto select_in_byte = [] -> std::array<std::uint8_t, byte_values * byte_digits> {
        auto table = std::array<std::uint8_t, byte_values * byte_digits>{};
        for (auto const byte : std::views::iota(0UZ, byte_values)) {
                auto rank = 0UZ;
                for (auto const bit : std::views::iota(0U, byte_digits)) {
                        if (((byte >> bit) & 1UZ) != 0UZ) {
                                table[byte | (rank << byte_digits)] = static_cast<std::uint8_t>(bit);
                                ++rank;
                        }
                }
        }
        return table;
}();

// Vigna's broadword select: byte counts prefix-summed by one multiply, the byte found by one compare, then the table.
[[nodiscard]] constexpr auto select(bit_limb x, unsigned k) noexcept
        -> unsigned
{
        assert(std::cmp_less(k, std::popcount(x)));
        constexpr auto byte_mask     = byte_values - 1UZ;
        constexpr auto ones_per_byte = ~0UZ / byte_mask;
        constexpr auto byte_tops     = ones_per_byte << (byte_digits - 1U);
        constexpr auto pair_lows     = ~0UZ / 0x03UZ;
        constexpr auto nibble_lows   = ~0UZ / 0x05UZ;
        constexpr auto byte_lows     = ~0UZ / 0x11UZ;
        auto counts                  = x - ((x >> 1U) & pair_lows);
        counts                       = (counts & nibble_lows) + ((counts >> 2U) & nibble_lows);
        counts                       = ((counts + (counts >> 4U)) & byte_lows) * ones_per_byte;
        auto const bytes_below       = static_cast<unsigned>(std::popcount((((k * ones_per_byte) | byte_tops) - counts) & byte_tops));
        auto const offset            = byte_digits * bytes_below;
        auto const rank_in_byte      = k - static_cast<unsigned>(((counts << byte_digits) >> offset) & byte_mask);
        return offset + select_in_byte[static_cast<std::size_t>(((x >> offset) & byte_mask) | (rank_in_byte << byte_digits))];
}

} // namespace xstd::ints::detail::portable

#endif // XSTD_INTS_BIT_DETAIL_PORTABLE_SELECT_HPP
