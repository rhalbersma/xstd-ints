//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_EXPAND_HPP
#define XSTD_INTS_BIT_BIT_EXPAND_HPP

#include <xstd/ints/bit/detail/bit_limb.hpp>              // bit_limb, bit_limb_width, shift_count_t
#include <xstd/ints/bit/detail/expand_limb.hpp>           // expand_limb
#include <xstd/ints/bit/detail/std_permutes_bits.hpp>     // XSTD_HAS_STD_BIT_PERMUTATIONS, std_permutes_bits
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // bit_expand, popcount

// P3104R5's bit_expand, over every unsigned integer xstd knows.
namespace xstd {

#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS

template<class T>
        requires ints::detail::std_permutes_bits<T>
[[nodiscard]] constexpr auto bit_expand(T src, T mask) noexcept
        -> T
{
        return std::bit_expand(src, mask);
}

#endif

// Each word of the mask is expanded on its own, drawing on the bits of src the words below it left unconsumed.
template<unsigned_integer T>
        requires (not ints::detail::std_permutes_bits<T>)
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the source and then the mask
[[nodiscard]] constexpr auto bit_expand(T src, T mask) noexcept(nothrow_const_operators<T>)
        -> T
{
        using ints::detail::bit_limb;
        using shift_count = ints::detail::shift_count_t<T>;
        constexpr auto N  = shift_count{numeric_limits<T>::digits};
        constexpr auto L  = shift_count{ints::detail::bit_limb_width};
        auto dst          = T{0};
        auto consumed     = shift_count{0};
        for (auto k = shift_count{0}; k < N; k += L) {
                // Both kept bit_limbs until shifted: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const mask_limb = static_cast<bit_limb>(mask >> k);
                auto const deposited = ints::detail::expand_limb(static_cast<bit_limb>(src >> consumed), mask_limb);
                dst                  = static_cast<T>(dst | static_cast<T>(static_cast<T>(deposited) << k));
                consumed += static_cast<shift_count>(std::popcount(mask_limb));
        }
        return dst;
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_EXPAND_HPP
