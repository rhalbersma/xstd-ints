//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_BIT_PERMUTATION_HPP
#define XSTD_INTS_DETAIL_BIT_PERMUTATION_HPP

#include <xstd/ints/detail/bit_limb.hpp>  // IWYU pragma: export; bit_limb, bit_limb_width, shift_count_t
#include <xstd/ints/detail/intrinsic.hpp> // compress, expand, XSTD_INTS_HAS_FAST_BMI2
#include <xstd/ints/detail/portable.hpp>  // compress_by_rank, expand_by_walk
#include <bit>                            // bit_compress, bit_expand, bit_repeat, bit_reverse, byteswap
#include <cstdint>                        // uint32_t, uint64_t
#include <version>                        // IWYU pragma: keep; __cpp_lib_bitops

// The working draft's __cpp_lib_bitops for P3104, which puts the four permutations in <bit>.
#if defined(__cpp_lib_bitops) and __cpp_lib_bitops >= 202607L
#define XSTD_HAS_STD_BIT_PERMUTATIONS 1
#endif

// MSVC 2022 has no __has_builtin, so testing for the bit reversal builtins takes an #if of its own.
#ifdef __has_builtin
#if __has_builtin(__builtin_bitreverse32) and __has_builtin(__builtin_bitreverse64)
#define XSTD_HAS_BUILTIN_BITREVERSE 1
#endif
#endif

namespace xstd::ints::detail {

#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS

template<class T>
concept std_permutes_bits = requires (T x, int l) {
        std::bit_compress(x, x);
        std::bit_expand(x, x);
        std::bit_repeat(x, l);
        std::bit_reverse(x);
};

#else

template<class T>
concept std_permutes_bits = false;

#endif

[[nodiscard]] constexpr auto reverse_limb(bit_limb x) noexcept
        -> bit_limb
{
#ifdef XSTD_HAS_BUILTIN_BITREVERSE
        if constexpr (sizeof(bit_limb) == sizeof(std::uint64_t)) {
                return static_cast<bit_limb>(__builtin_bitreverse64(x));
        } else {
                return static_cast<bit_limb>(__builtin_bitreverse32(static_cast<std::uint32_t>(x)));
        }
#else
        // Mirroring the bytes leaves each byte's own bits to mirror: its nibbles, then its pairs, then its single bits.
        constexpr auto nibbles = static_cast<bit_limb>(~bit_limb{0} / 0x11U);
        constexpr auto pairs   = static_cast<bit_limb>(~bit_limb{0} / 0x05U);
        constexpr auto singles = static_cast<bit_limb>(~bit_limb{0} / 0x03U);
        x                      = std::byteswap(x);
        x                      = ((x >> 4U) & nibbles) | ((x & nibbles) << 4U);
        x                      = ((x >> 2U) & pairs) | ((x & pairs) << 2U);
        x                      = ((x >> 1U) & singles) | ((x & singles) << 1U);
        return x;
#endif
}

// The hardware instruction at run time where the target makes it fast, the portable kernel everywhere else.
[[nodiscard]] constexpr auto compress_limb(bit_limb x, bit_limb m) noexcept
        -> bit_limb
{
#ifdef XSTD_INTS_HAS_FAST_BMI2
        if not consteval {
                return intrinsic::compress(x, m);
        }
#endif
        return portable::compress_by_rank(x, m);
}

// The hardware instruction at run time where the target makes it fast, the portable kernel everywhere else.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] constexpr auto expand_limb(bit_limb x, bit_limb m) noexcept
        -> bit_limb
{
#ifdef XSTD_INTS_HAS_FAST_BMI2
        if not consteval {
                return intrinsic::expand(x, m);
        }
#endif
        return portable::expand_by_walk(x, m);
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_DETAIL_BIT_PERMUTATION_HPP
