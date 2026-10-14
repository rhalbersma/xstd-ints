//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_INTRINSIC_BLOCK_REVERSE_HPP
#define XSTD_INTS_BIT_DETAIL_INTRINSIC_BLOCK_REVERSE_HPP

// MSVC 2022 has no __has_builtin, so testing for the bit reversal builtins takes an #if of its own.
#ifdef __has_builtin
#if __has_builtin(__builtin_bitreverse32) and __has_builtin(__builtin_bitreverse64)
#define XSTD_INTS_HAS_BUILTIN_BITREVERSE 1
#endif
#endif

#ifdef XSTD_INTS_HAS_BUILTIN_BITREVERSE

namespace xstd::ints::detail::intrinsic {

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
