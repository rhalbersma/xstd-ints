//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_POPCOUNT_HPP
#define XSTD_INTS_BIT_POPCOUNT_HPP

#include <xstd/ints/bit/detail/bit_block.hpp>             // bit_block, bit_block_width, shift_count_t
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/cstdint/bit_int.hpp>                  // XSTD_HAS_BIT_INT, bit_uint
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // popcount
#include <cstddef>                                        // size_t

// One overload set: <bit> where it takes the type, a builtin for the bit-precise widths it refuses, blocks for the rest.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::popcount(x); }
[[nodiscard]] constexpr auto popcount(T x) noexcept
        -> int
{
        return std::popcount(x);
}

#ifdef XSTD_HAS_BIT_INT
#if __has_builtin(__builtin_popcountg)

// Where <bit> refuses unsigned _BitInt(N), as libstdc++ does, the type-generic builtin counts every width.
template<std::size_t N>
        requires (not requires (bit_uint<N> x) { std::popcount(x); })
[[nodiscard]] constexpr auto popcount(bit_uint<N> x) noexcept
        -> int
{
        return __builtin_popcountg(x);
}

#endif
#endif

// Each block's population, summed.
template<unsigned_integer T>
        requires (not requires (T x) { std::popcount(x); })
[[nodiscard]] constexpr auto popcount(T x) noexcept(nothrow_const_operators<T>)
        -> int
{
        using ints::detail::bit_block;
        using shift_count = ints::detail::shift_count_t<T>;
        constexpr auto N  = static_cast<std::size_t>(numeric_limits<T>::digits);
        constexpr auto L  = ints::detail::bit_block_width;
        auto count        = 0;
        for (auto k = 0UZ; k < N; k += L) {
                count += std::popcount(static_cast<bit_block>(x >> static_cast<shift_count>(k)));
        }
        return count;
}

} // namespace xstd

#endif // XSTD_INTS_BIT_POPCOUNT_HPP
