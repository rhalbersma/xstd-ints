//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_COUNTR_ZERO_HPP
#define XSTD_INTS_BIT_COUNTR_ZERO_HPP

#include <xstd/ints/cstdint/bit_int.hpp> // XSTD_HAS_BIT_INT, bit_uint
#include <bit>                           // countr_zero
#include <cstddef>                       // size_t

// One overload set per function: <bit> takes std::unsigned_integral, which every 128-bit integer class fails.
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

} // namespace xstd

#endif // XSTD_INTS_BIT_COUNTR_ZERO_HPP
