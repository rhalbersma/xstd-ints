//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_REVERSE_HPP
#define XSTD_INTS_BIT_BIT_REVERSE_HPP

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/concepts/unsigned_integer.hpp>        // unsigned_integer
#include <xstd/ints/detail/bit_limb.hpp>                  // mirror_limbs
#include <xstd/ints/detail/bit_permutation.hpp>           // XSTD_HAS_STD_BIT_PERMUTATIONS, reverse_limb, std_permutes_bits
#include <bit>                                            // IWYU pragma: keep; bit_reverse, where the standard library ships it

// P3104R5's bit_reverse, over every unsigned integer xstd knows.
namespace xstd {

#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS

template<class T>
        requires ints::detail::std_permutes_bits<T>
[[nodiscard]] constexpr auto bit_reverse(T x) noexcept
        -> T
{
        return std::bit_reverse(x);
}

#endif

// Mirroring each word's bits, so bit k of x becomes bit N - 1 - k.
template<unsigned_integer T>
        requires (not ints::detail::std_permutes_bits<T>)
[[nodiscard]] constexpr auto bit_reverse(T x) noexcept(nothrow_const_operators<T>)
        -> T
{
        return ints::detail::mirror_limbs(x, ints::detail::reverse_limb);
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_REVERSE_HPP
