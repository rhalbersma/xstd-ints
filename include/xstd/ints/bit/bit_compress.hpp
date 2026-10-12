//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_COMPRESS_HPP
#define XSTD_INTS_BIT_BIT_COMPRESS_HPP

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/detail/bit_limb.hpp>                  // bit_limb, bit_limb_width, shift_count_t
#include <xstd/ints/detail/compress_limb.hpp>             // compress_limb
#include <xstd/ints/detail/std_permutes_bits.hpp>         // XSTD_HAS_STD_BIT_PERMUTATIONS, std_permutes_bits
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // bit_compress, popcount

// P3104R5's bit_compress, over every unsigned integer xstd knows.
namespace xstd {

#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS

template<class T>
        requires ints::detail::std_permutes_bits<T>
[[nodiscard]] constexpr auto bit_compress(T src, T mask) noexcept
        -> T
{
        return std::bit_compress(src, mask);
}

#endif

// Each word is compressed on its own and packed directly above the bits the words below it selected.
template<unsigned_integer T>
        requires (not ints::detail::std_permutes_bits<T>)
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the source and then the mask
[[nodiscard]] constexpr auto bit_compress(T src, T mask) noexcept(nothrow_const_operators<T>)
        -> T
{
        using ints::detail::bit_limb;
        using shift_count = ints::detail::shift_count_t<T>;
        constexpr auto N  = shift_count{numeric_limits<T>::digits};
        constexpr auto L  = shift_count{ints::detail::bit_limb_width};
        auto dst          = T{0};
        auto selected     = shift_count{0};
        for (auto k = shift_count{0}; k < N; k += L) {
                // Both kept bit_limbs until shifted: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const mask_limb = static_cast<bit_limb>(mask >> k);
                auto const packed    = ints::detail::compress_limb(static_cast<bit_limb>(src >> k), mask_limb);
                dst                  = static_cast<T>(dst | static_cast<T>(static_cast<T>(packed) << selected));
                selected += static_cast<shift_count>(std::popcount(mask_limb));
        }
        return dst;
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_COMPRESS_HPP
