//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_PORTABLE_SELECT_HPP
#define XSTD_INTS_DETAIL_PORTABLE_SELECT_HPP

#include <xstd/ints/detail/bit_limb.hpp> // bit_limb
#include <array>                         // array
#include <bit>                           // popcount
#include <cassert>                       // assert
#include <cstddef>                       // size_t
#include <cstdint>                       // uint8_t
#include <utility>                       // cmp_less

namespace xstd::ints::detail::portable {

// The position of the k-th one of each byte value, at index byte | (k << 8).
inline constexpr auto select_in_byte = [] -> std::array<std::uint8_t, 256UZ * 8UZ> {
        auto table = std::array<std::uint8_t, 256UZ * 8UZ>{};
        for (auto byte = 0UZ; byte < 256UZ; ++byte) {
                auto rank = 0UZ;
                for (auto bit = 0UZ; bit < 8UZ; ++bit) {
                        if (((byte >> bit) & 1UZ) != 0UZ) {
                                table[byte | (rank << 8UZ)] = static_cast<std::uint8_t>(bit);
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
        constexpr auto ones_per_byte = ~bit_limb{0} / 0xFFU;
        constexpr auto byte_tops     = ones_per_byte << 7U;
        auto counts                  = x - ((x >> 1U) & (~bit_limb{0} / 0x03U));
        counts                       = (counts & (~bit_limb{0} / 0x05U)) + ((counts >> 2U) & (~bit_limb{0} / 0x05U));
        counts                       = ((counts + (counts >> 4U)) & (~bit_limb{0} / 0x11U)) * ones_per_byte;
        auto const bytes_below       = static_cast<unsigned>(std::popcount((((k * ones_per_byte) | byte_tops) - counts) & byte_tops));
        auto const offset            = 8U * bytes_below;
        auto const rank_in_byte      = k - static_cast<unsigned>(((counts << 8U) >> offset) & 0xFFU);
        return offset + select_in_byte[static_cast<std::size_t>(((x >> offset) & 0xFFU) | (rank_in_byte << 8U))];
}

} // namespace xstd::ints::detail::portable

#endif // XSTD_INTS_DETAIL_PORTABLE_SELECT_HPP
