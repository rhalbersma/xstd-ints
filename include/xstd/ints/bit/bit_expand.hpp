//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_EXPAND_HPP
#define XSTD_INTS_BIT_BIT_EXPAND_HPP

#include <xstd/ints/bit/detail/block_expand.hpp>          // block_expand
#include <xstd/ints/bit/detail/block_width.hpp>           // block_width
#include <xstd/ints/bit/detail/shift_count.hpp>           // shift_count_t
#include <xstd/ints/bit/detail/std_permutes_bits.hpp>     // XSTD_HAS_STD_BIT_COMPRESS, std_compresses_bits
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // bit_expand, popcount
#include <cstddef>                                        // size_t

// P3104R5's bit_expand, over every unsigned integer xstd knows.
namespace xstd {

#ifdef XSTD_HAS_STD_BIT_COMPRESS

template<class T>
        requires ints::detail::std_compresses_bits<T>
[[nodiscard]] constexpr auto bit_expand(T src, T mask) noexcept
        -> T
{
        return std::bit_expand(src, mask);
}

#endif

// Each block of the mask is expanded on its own, drawing on the bits of src the blocks below it left unconsumed.
template<unsigned_integer T>
        requires (not ints::detail::std_compresses_bits<T>)
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the source and then the mask
[[nodiscard]] constexpr auto bit_expand(T src, T mask) noexcept(nothrow_const_operators<T>)
        -> T
{
        using shift_count = ints::detail::shift_count_t<T>;
        constexpr auto N  = shift_count{numeric_limits<T>::digits};
        constexpr auto L  = shift_count{ints::detail::block_width};
        auto dst          = T{0};
        auto consumed     = shift_count{0};
        for (auto k = shift_count{0}; k < N; k += L) {
                // Both kept std::size_t until shifted: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const mask_block = static_cast<std::size_t>(mask >> k);
                auto const deposited  = ints::detail::block_expand(static_cast<std::size_t>(src >> consumed), mask_block);
                dst                   = static_cast<T>(dst | static_cast<T>(static_cast<T>(deposited) << k));
                consumed += static_cast<shift_count>(std::popcount(mask_block));
        }
        return dst;
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_EXPAND_HPP
