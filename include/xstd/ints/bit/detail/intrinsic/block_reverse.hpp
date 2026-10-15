//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_INTRINSIC_BLOCK_REVERSE_HPP
#define XSTD_INTS_BIT_DETAIL_INTRINSIC_BLOCK_REVERSE_HPP

#include <xstd/ints/bit/detail/intrinsic/gfni.hpp> // IWYU pragma: keep; XSTD_INTS_HAS_GFNI and its intrinsics
#include <xstd/ints/bit/detail/intrinsic/rbit.hpp> // IWYU pragma: keep; XSTD_INTS_HAS_RBIT and its intrinsics

// MSVC 2022 has no __has_builtin, so testing for the bit reversal builtins takes an #if of its own.
#ifdef __has_builtin
#if __has_builtin(__builtin_bitreverse32) and __has_builtin(__builtin_bitreverse64)
#define XSTD_INTS_HAS_BUILTIN_BITREVERSE 1
#endif
#endif

// Only the GFNI kernel puts the mirrored bytes back in order, and a builtin takes precedence over it.
#if defined(XSTD_INTS_HAS_GFNI) and not defined(XSTD_INTS_HAS_BUILTIN_BITREVERSE)
#include <bit> // byteswap
#endif

#if defined(XSTD_INTS_HAS_BUILTIN_BITREVERSE) or defined(XSTD_INTS_HAS_RBIT) or defined(XSTD_INTS_HAS_GFNI)

namespace xstd::ints::detail::intrinsic {

#ifdef XSTD_INTS_HAS_BUILTIN_BITREVERSE

// __builtin_bitreverse32's own width, and like it a constant expression.
[[nodiscard]] constexpr auto block_reverse(unsigned int x) noexcept
        -> unsigned int
{
        return __builtin_bitreverse32(x);
}

// __builtin_bitreverse64's own width, and like it a constant expression.
[[nodiscard]] constexpr auto block_reverse(unsigned long long x) noexcept
        -> unsigned long long
{
        return __builtin_bitreverse64(x);
}

#elifdef XSTD_INTS_HAS_RBIT

// __rbit's own width.
[[nodiscard]] inline auto block_reverse(unsigned int x) noexcept
        -> unsigned int
{
        return __rbit(x);
}

// __rbitll's own width.
[[nodiscard]] inline auto block_reverse(unsigned long long x) noexcept
        -> unsigned long long
{
        return __rbitll(x);
}

#else

// Each row of the affine matrix selects the source bit that lands in the mirrored position of every byte.
inline constexpr auto mirror_each_byte = static_cast<long long>(0x8040'2010'0804'0201ULL);

// The bits of every byte mirrored by one affine transform, then the bytes by one swap.
[[nodiscard]] inline auto block_reverse(unsigned int x) noexcept
        -> unsigned int
{
        auto const bytes = _mm_gf2p8affine_epi64_epi8(_mm_cvtsi32_si128(static_cast<int>(x)), _mm_set1_epi64x(mirror_each_byte), 0);
        return std::byteswap(static_cast<unsigned int>(_mm_cvtsi128_si32(bytes)));
}

// The bits of every byte mirrored by one affine transform, then the bytes by one swap.
[[nodiscard]] inline auto block_reverse(unsigned long long x) noexcept
        -> unsigned long long
{
        auto const bytes = _mm_gf2p8affine_epi64_epi8(_mm_cvtsi64_si128(static_cast<long long>(x)), _mm_set1_epi64x(mirror_each_byte), 0);
        return std::byteswap(static_cast<unsigned long long>(_mm_cvtsi128_si64(bytes)));
}

#endif

// A type of its own ([basic.fundamental]/2) as wide as one of the two above: 64 bits on LP64, 32 on LLP64.
[[nodiscard]] constexpr auto block_reverse(unsigned long x) noexcept
        -> unsigned long
{
        // Outside a template a discarded branch is still checked, so the requirement is stated once, before either.
        static_assert(sizeof(unsigned long) == sizeof(unsigned int) or sizeof(unsigned long) == sizeof(unsigned long long));
        if constexpr (sizeof(unsigned long) == sizeof(unsigned int)) {
                return static_cast<unsigned long>(block_reverse(static_cast<unsigned int>(x)));
        } else {
                return static_cast<unsigned long>(block_reverse(static_cast<unsigned long long>(x)));
        }
}

} // namespace xstd::ints::detail::intrinsic

#endif

#endif // XSTD_INTS_BIT_DETAIL_INTRINSIC_BLOCK_REVERSE_HPP
