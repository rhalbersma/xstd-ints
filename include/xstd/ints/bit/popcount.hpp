//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_POPCOUNT_HPP
#define XSTD_INTS_BIT_POPCOUNT_HPP

#include <xstd/ints/cstdint/bit_int.hpp> // XSTD_HAS_BIT_INT, bit_uint
#include <bit>                           // popcount
#include <cstddef>                       // size_t

// One overload set per function: <bit> takes std::unsigned_integral, which every 128-bit integer class fails.
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

} // namespace xstd

#endif // XSTD_INTS_BIT_POPCOUNT_HPP
