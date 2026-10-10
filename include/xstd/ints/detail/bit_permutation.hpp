//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_BIT_PERMUTATION_HPP
#define XSTD_INTS_DETAIL_BIT_PERMUTATION_HPP

#include <bit>         // bit_compress, bit_expand, bit_repeat, bit_reverse, byteswap
#include <cstddef>     // size_t
#include <cstdint>     // uint32_t, uint64_t
#include <limits>      // numeric_limits
#include <type_traits> // conditional_t, is_class_v
#include <version>     // __cpp_lib_bitops

// Chosen at compile time: PEXT and PDEP are microcoded on AMD before Zen 3, so builds for those omit -mbmi2.
#if defined(__BMI2__) and defined(__x86_64__)
#include <immintrin.h> // _pdep_u64, _pext_u64
#endif

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
        std::bit_reverse(x);
        std::bit_repeat(x, l);
        std::bit_compress(x, x);
        std::bit_expand(x, x);
};

#else

template<class T>
concept std_permutes_bits = false;

#endif

// Every integer-class type converts to and from std::size_t, so every unsigned integer splits into these words.
using bit_limb = std::size_t;

inline constexpr auto bit_limb_width = static_cast<std::size_t>(std::numeric_limits<bit_limb>::digits);

// Unsigned for a builtin shift, so no count is signed; int for a class type's, as absl::uint128's operator<< takes.
template<class T>
using shift_count_t = std::conditional_t<std::is_class_v<T>, int, unsigned>;

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

// Each one-bit of the mask, lowest first, moves the bit of x beneath it to the next free position of the result.
[[nodiscard]] constexpr auto compress_limb(bit_limb x, bit_limb m) noexcept
        -> bit_limb
{
#if defined(__BMI2__) and defined(__x86_64__)
        if not consteval {
                return static_cast<bit_limb>(_pext_u64(x, m));
        }
#endif
        auto result = bit_limb{0};
        for (auto next = bit_limb{1}; m != bit_limb{0}; next <<= 1U) {
                auto const higher = m & (m - 1U);
                if ((x & (m ^ higher)) != bit_limb{0}) {
                        result |= next;
                }
                m = higher;
        }
        return result;
}

// The inverse walk: each one-bit of the mask, lowest first, receives the next bit of x.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] constexpr auto expand_limb(bit_limb x, bit_limb m) noexcept
        -> bit_limb
{
#if defined(__BMI2__) and defined(__x86_64__)
        if not consteval {
                return static_cast<bit_limb>(_pdep_u64(x, m));
        }
#endif
        auto result = bit_limb{0};
        for (auto next = bit_limb{1}; m != bit_limb{0}; next <<= 1U) {
                auto const higher = m & (m - 1U);
                if ((x & next) != bit_limb{0}) {
                        result |= m ^ higher;
                }
                m = higher;
        }
        return result;
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_DETAIL_BIT_PERMUTATION_HPP
