//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_MIRROR_BLOCKS_HPP
#define XSTD_INTS_BIT_DETAIL_MIRROR_BLOCKS_HPP

#include <xstd/ints/bit/detail/block_width.hpp>           // block_width
#include <xstd/ints/bit/detail/shift_count.hpp>           // shift_count_t
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <cstddef>                                        // size_t

namespace xstd::ints::detail {

// Each block is mirrored on its own and lands at the mirror of its offset: a block's lowest unit becomes the highest.
template<class T, class Mirror>
[[nodiscard]] constexpr auto mirror_blocks(T x, Mirror mirror) noexcept(nothrow_const_operators<T>)
        -> T
{
        using shift_count      = shift_count_t<T>;
        constexpr auto N       = shift_count{xstd::numeric_limits<T>::digits};
        constexpr auto L       = shift_count{block_width};
        constexpr auto partial = N % L;
        auto result            = T{0};
        for (auto k = shift_count{0}; k < N - partial; k += L) {
                // Kept a std::size_t until shifted: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const mirrored = mirror(static_cast<std::size_t>(x >> k));
                result              = static_cast<T>(result | static_cast<T>(static_cast<T>(mirrored) << (N - L - k)));
        }
        if constexpr (partial != 0) {
                constexpr auto slack = unsigned{L - partial};
                auto const mirrored  = mirror(static_cast<std::size_t>(x >> (N - partial)));
                result               = static_cast<T>(result | static_cast<T>(mirrored >> slack));
        }
        return result;
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_MIRROR_BLOCKS_HPP
