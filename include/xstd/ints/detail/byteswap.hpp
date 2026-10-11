//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_BYTESWAP_HPP
#define XSTD_INTS_DETAIL_BYTESWAP_HPP

#include <bit>         // byteswap
#include <type_traits> // has_unique_object_representations_v

namespace xstd::ints::detail {

// [bit.byteswap] mandates no padding bits, which a bit-precise width that fills no whole object has.
template<class T>
concept std_swaps_bytes = std::has_unique_object_representations_v<T> and requires (T x) { std::byteswap(x); };

} // namespace xstd::ints::detail

#endif // XSTD_INTS_DETAIL_BYTESWAP_HPP
