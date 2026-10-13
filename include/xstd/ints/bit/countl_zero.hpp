//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_COUNTL_ZERO_HPP
#define XSTD_INTS_BIT_COUNTL_ZERO_HPP

#include <xstd/ints/bit/detail/block_width.hpp>           // block_width
#include <xstd/ints/bit/detail/shift_count.hpp>           // shift_count_t
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/cstdint/bit_int.hpp>                  // XSTD_HAS_BIT_INT, bit_uint
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <bit>                                            // bit_width, countl_zero
#include <cstddef>                                        // size_t

// One overload set: <bit> where it takes the type, a builtin for the bit-precise widths it refuses, blocks for the rest.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::countl_zero(x); }
[[nodiscard]] constexpr auto countl_zero(T x) noexcept
        -> int
{
        return std::countl_zero(x);
}

#ifdef XSTD_HAS_BIT_INT
#if __has_builtin(__builtin_clzg)

// Where <bit> refuses unsigned _BitInt(N), as libstdc++ does, the builtin's second argument answers zero.
template<std::size_t N>
        requires (not requires (bit_uint<N> x) { std::countl_zero(x); })
[[nodiscard]] constexpr auto countl_zero(bit_uint<N> x) noexcept
        -> int
{
        return __builtin_clzg(x, static_cast<int>(N));
}

#endif
#endif

// The highest block holding a one decides, its leading zeros counted down from the top of the width.
template<unsigned_integer T>
        requires (not requires (T x) { std::countl_zero(x); })
[[nodiscard]] constexpr auto countl_zero(T x) noexcept(nothrow_const_operators<T>)
        -> int
{
        using shift_count = ints::detail::shift_count_t<T>;
        constexpr auto N  = static_cast<std::size_t>(numeric_limits<T>::digits);
        constexpr auto L  = ints::detail::block_width;
        constexpr auto K  = (N + L - 1UZ) / L;
        for (auto i = K - 1UZ; i < K; --i) {
                auto const block = static_cast<std::size_t>(x >> static_cast<shift_count>(i * L));
                if (block != std::size_t{0}) {
                        return static_cast<int>(N - (i * L)) - std::bit_width(block);
                }
        }
        return static_cast<int>(N);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_COUNTL_ZERO_HPP
