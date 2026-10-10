//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_EXPAND_HPP
#define XSTD_INTS_BIT_BIT_EXPAND_HPP

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/detail/bit_permutation.hpp>           // XSTD_HAS_STD_BIT_PERMUTATIONS, bit_limb, bit_limb_width, expand_limb, std_permutes_bits
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // bit_expand, popcount

// P3104R5's bit_expand, over every unsigned integer xstd knows.
namespace xstd {

#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS

template<class T>
        requires ints::detail::std_permutes_bits<T>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] constexpr auto bit_expand(T x, T m) noexcept
        -> T
{
        return std::bit_expand(x, m);
}

#endif

// Each word of the mask is expanded on its own, drawing on the bits of x the words below it left unconsumed.
template<unsigned_integer T>
        requires (not ints::detail::std_permutes_bits<T>)
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] constexpr auto bit_expand(T x, T m) noexcept(nothrow_const_operators<T>)
        -> T
{
        using ints::detail::bit_limb;
        constexpr auto N = numeric_limits<T>::digits;
        constexpr auto L = static_cast<int>(ints::detail::bit_limb_width);
        auto result      = T{0};
        auto consumed    = 0;
        for (auto k = 0; k < N; k += L) {
                auto const mask      = static_cast<bit_limb>(m >> k);
                auto const deposited = static_cast<T>(ints::detail::expand_limb(static_cast<bit_limb>(x >> consumed), mask));
                result               = static_cast<T>(result | static_cast<T>(deposited << k));
                consumed += std::popcount(mask);
        }
        return result;
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_EXPAND_HPP
