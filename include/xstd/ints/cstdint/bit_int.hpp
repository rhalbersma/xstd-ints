//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDINT_BIT_INT_HPP
#define XSTD_INTS_CSTDINT_BIT_INT_HPP

#include <xstd/ints/limits/numeric_limits.hpp>     // IWYU pragma: export; numeric_limits<_BitInt(N)>
#include <xstd/ints/type_traits/make_signed.hpp>   // IWYU pragma: export; make_signed<unsigned _BitInt(N)>
#include <xstd/ints/type_traits/make_unsigned.hpp> // IWYU pragma: export; make_unsigned<signed _BitInt(N)>
#include <cstddef>                                 // size_t

#ifdef __BITINT_MAXWIDTH__

#define XSTD_HAS_BIT_INT 1

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbit-int-extension"
#endif

namespace xstd {

inline constexpr auto bit_int_max_width = static_cast<std::size_t>(__BITINT_MAXWIDTH__);

// Keep the public domain paired until Clang implements signed _BitInt(1) from N3747.
template<std::size_t N>
        requires (2 <= N and N <= bit_int_max_width)
using bit_int = signed _BitInt(N);

template<std::size_t N>
        requires (2 <= N and N <= bit_int_max_width)
using bit_uint = unsigned _BitInt(N);

} // namespace xstd

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // __BITINT_MAXWIDTH__

#endif // XSTD_INTS_CSTDINT_BIT_INT_HPP
