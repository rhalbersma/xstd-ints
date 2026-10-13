//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_BLOCK_WIDTH_HPP
#define XSTD_INTS_BIT_DETAIL_BLOCK_WIDTH_HPP

#include <cstddef> // size_t
#include <limits>  // numeric_limits

namespace xstd::ints::detail {

// Every integer-class type converts to and from std::size_t, so every unsigned integer splits into blocks this wide.
inline constexpr auto block_width = static_cast<std::size_t>(std::numeric_limits<std::size_t>::digits);

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_BLOCK_WIDTH_HPP
