//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_BLOCK_REVERSE_HPP
#define XSTD_INTS_BIT_DETAIL_BLOCK_REVERSE_HPP

#include <xstd/ints/bit/detail/bit_block.hpp> // bit_block
#include <bit>                                // IWYU pragma: keep; byteswap, where no bit reversal builtin exists
#include <cstdint>                            // uint32_t, uint64_t

// MSVC 2022 has no __has_builtin, so testing for the bit reversal builtins takes an #if of its own.
#ifdef __has_builtin
#if __has_builtin(__builtin_bitreverse32) and __has_builtin(__builtin_bitreverse64)
#define XSTD_HAS_BUILTIN_BITREVERSE 1
#endif
#endif

namespace xstd::ints::detail {

[[nodiscard]] constexpr auto block_reverse(bit_block x) noexcept
        -> bit_block
{
#ifdef XSTD_HAS_BUILTIN_BITREVERSE
        if constexpr (sizeof(bit_block) == sizeof(std::uint64_t)) {
                return static_cast<bit_block>(__builtin_bitreverse64(x));
        } else {
                return static_cast<bit_block>(__builtin_bitreverse32(static_cast<std::uint32_t>(x)));
        }
#else
        // Mirroring the bytes leaves each byte's own bits to mirror: its nibbles, then its pairs, then its single bits.
        constexpr auto nibbles = ~0UZ / 0x11UZ;
        constexpr auto pairs   = ~0UZ / 0x05UZ;
        constexpr auto singles = ~0UZ / 0x03UZ;
        x                      = std::byteswap(x);
        x                      = ((x >> 4U) & nibbles) | ((x & nibbles) << 4U);
        x                      = ((x >> 2U) & pairs) | ((x & pairs) << 2U);
        x                      = ((x >> 1U) & singles) | ((x & singles) << 1U);
        return x;
#endif
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_BLOCK_REVERSE_HPP
