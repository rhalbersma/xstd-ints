//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_REVERSE_HPP
#define XSTD_INTS_BIT_BIT_REVERSE_HPP

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/detail/bit_permutation.hpp>           // XSTD_HAS_STD_BIT_PERMUTATIONS, bit_limb, bit_limb_width, reverse_limb, std_permutes_bits
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // bit_reverse

// P3104R5's bit_reverse, over every unsigned integer xstd knows.
namespace xstd {

#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS

template<class T>
        requires ints::detail::std_permutes_bits<T>
[[nodiscard]] constexpr auto bit_reverse(T x) noexcept
        -> T
{
        return std::bit_reverse(x);
}

#endif

// Each word is mirrored on its own and lands at the mirror of its offset, so bit k of x becomes bit N - 1 - k.
template<unsigned_integer T>
        requires (not ints::detail::std_permutes_bits<T>)
[[nodiscard]] constexpr auto bit_reverse(T x) noexcept(nothrow_const_operators<T>)
        -> T
{
        using ints::detail::bit_limb;
        constexpr auto N       = numeric_limits<T>::digits;
        constexpr auto L       = static_cast<int>(ints::detail::bit_limb_width);
        constexpr auto partial = N % L;
        auto result            = T{0};
        for (auto k = 0; k < N - partial; k += L) {
                // Kept a bit_limb until shifted: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const mirrored = ints::detail::reverse_limb(static_cast<bit_limb>(x >> k));
                result              = static_cast<T>(result | static_cast<T>(static_cast<T>(mirrored) << (N - L - k)));
        }
        if constexpr (partial != 0) {
                auto const mirrored = ints::detail::reverse_limb(static_cast<bit_limb>(x >> (N - partial)));
                result              = static_cast<T>(result | static_cast<T>(mirrored >> (L - partial)));
        }
        return result;
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_REVERSE_HPP
