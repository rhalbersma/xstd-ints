//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_COUNTR_ZERO_HPP
#define XSTD_INTS_BIT_COUNTR_ZERO_HPP

#include <xstd/ints/bit/detail/bit_block.hpp>             // bit_block, bit_block_width, shift_count_t
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/cstdint/bit_int.hpp>                  // XSTD_HAS_BIT_INT, bit_uint
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // countr_zero
#include <cstddef>                                        // size_t

// One overload set: <bit> where it takes the type, a builtin for the bit-precise widths it refuses, blocks for the rest.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::countr_zero(x); }
[[nodiscard]] constexpr auto countr_zero(T x) noexcept
        -> int
{
        return std::countr_zero(x);
}

#ifdef XSTD_HAS_BIT_INT
#if __has_builtin(__builtin_ctzg)

// Where <bit> refuses unsigned _BitInt(N), as libstdc++ does, the builtin's second argument answers zero.
template<std::size_t N>
        requires (not requires (bit_uint<N> x) { std::countr_zero(x); })
[[nodiscard]] constexpr auto countr_zero(bit_uint<N> x) noexcept
        -> int
{
        return __builtin_ctzg(x, static_cast<int>(N));
}

#endif
#endif

// The lowest block holding a one decides, its trailing zeros counted up from its offset.
template<unsigned_integer T>
        requires (not requires (T x) { std::countr_zero(x); })
[[nodiscard]] constexpr auto countr_zero(T x) noexcept(nothrow_const_operators<T>)
        -> int
{
        using ints::detail::bit_block;
        using shift_count = ints::detail::shift_count_t<T>;
        constexpr auto N  = static_cast<std::size_t>(numeric_limits<T>::digits);
        constexpr auto L  = ints::detail::bit_block_width;
        for (auto k = 0UZ; k < N; k += L) {
                auto const block = static_cast<bit_block>(x >> static_cast<shift_count>(k));
                if (block != bit_block{0}) {
                        return static_cast<int>(k) + std::countr_zero(block);
                }
        }
        return static_cast<int>(N);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_COUNTR_ZERO_HPP
