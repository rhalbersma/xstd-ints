//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BYTESWAP_HPP
#define XSTD_INTS_BIT_BYTESWAP_HPP

#include <xstd/ints/bit/detail/bit_limb.hpp>              // bit_limb, mirror_limbs
#include <xstd/ints/bit/detail/byteswap.hpp>              // std_swaps_bytes
#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <xstd/ints/type_traits/make_unsigned.hpp>        // make_unsigned_t
#include <bit>                                            // byteswap

// [bit.byteswap] over every integer xstd knows whose width is a whole number of bytes.
namespace xstd {

template<class T>
        requires ints::detail::std_swaps_bytes<T>
[[nodiscard]] constexpr auto byteswap(T x) noexcept
        -> T
{
        return std::byteswap(x);
}

// Through the unsigned counterpart, each word's bytes mirrored as bit_reverse mirrors its bits.
template<integer T>
        requires (not ints::detail::std_swaps_bytes<T>) and (numeric_limits<make_unsigned_t<T>>::digits % numeric_limits<unsigned char>::digits == 0)
[[nodiscard]] constexpr auto byteswap(T x) noexcept(nothrow_const_operators<T> and nothrow_const_operators<make_unsigned_t<T>>)
        -> T
{
        using ints::detail::bit_limb;
        return static_cast<T>(ints::detail::mirror_limbs(static_cast<make_unsigned_t<T>>(x), [](bit_limb limb) noexcept -> bit_limb { return std::byteswap(limb); }));
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BYTESWAP_HPP
