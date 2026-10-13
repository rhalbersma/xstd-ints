//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_SELECT_HPP
#define XSTD_INTS_BIT_BIT_SELECT_HPP

#include <xstd/ints/bit/detail/block_select.hpp>   // block_select
#include <xstd/ints/bit/detail/block_width.hpp>    // block_width
#include <xstd/ints/bit/detail/shift_count.hpp>    // shift_count_t
#include <xstd/ints/bit/popcount.hpp>              // popcount
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <bit>                                     // popcount
#include <cassert>                                 // assert
#include <cstddef>                                 // size_t
#include <utility>                                 // unreachable

// An xstd extension beside P3104's permutations, not a standard function: rank's inverse, with a narrow contract.
namespace xstd {

// The position of x's k-th one, counting from zero at the least significant end.
template<unsigned_integer T>
[[nodiscard]] constexpr auto bit_select(T x, int k)
        -> int
{
        assert(0 <= k and k < xstd::popcount(x));
        // As P3104R6 has it for bit_repeat: a call violating the precondition is no constant expression, NDEBUG or not.
        if consteval {
                if (k < 0 or k >= xstd::popcount(x)) {
                        std::unreachable();
                }
        }
        if constexpr (numeric_limits<T>::digits <= ints::detail::block_width) {
                return static_cast<int>(ints::detail::block_select(static_cast<std::size_t>(x), static_cast<unsigned>(k)));
        } else {
                // Whole blocks are skipped by their population until the k-th one lies in the current block.
                using shift_count = ints::detail::shift_count_t<T>;
                constexpr auto N  = shift_count{numeric_limits<T>::digits};
                constexpr auto L  = shift_count{ints::detail::block_width};
                // The precondition puts the k-th one in the top block once every block below it is skipped.
                constexpr auto top = static_cast<shift_count>((N - 1) / L * L);
                auto rank          = static_cast<unsigned>(k);
                auto offset        = shift_count{0};
                auto block         = static_cast<std::size_t>(x);
                auto ones          = static_cast<unsigned>(std::popcount(block));
                while (offset < top and rank >= ones) {
                        rank -= ones;
                        offset += L;
                        block = static_cast<std::size_t>(x >> offset);
                        ones  = static_cast<unsigned>(std::popcount(block));
                }
                return static_cast<int>(offset) + static_cast<int>(ints::detail::block_select(block, rank));
        }
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_SELECT_HPP
