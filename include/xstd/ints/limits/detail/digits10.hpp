//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_LIMITS_DETAIL_DIGITS10_HPP
#define XSTD_INTS_LIMITS_DETAIL_DIGITS10_HPP

#include <cstdint> // uint32_t, uint64_t
#include <limits>  // numeric_limits

namespace xstd::ints::detail {

// The widest digits for which digits10 below is exact, every width up to it checked against floor(digits * log10(2)).
inline constexpr auto digits10_exact_width = 8'388'608;

// floor(digits * log10(2)) in 64.64 fixed point, in 32-bit halves so that no product passes 64 bits.
[[nodiscard]] constexpr auto digits10(int digits) noexcept
        -> int
{
        constexpr auto log10_2 = std::uint64_t{0x4d10'4d42'7de7'fbcc}; // floor(log10(2) * 2^64)
        constexpr auto half    = std::numeric_limits<std::uint32_t>::digits;
        constexpr auto low     = std::uint64_t{std::numeric_limits<std::uint32_t>::max()};
        auto const d           = static_cast<std::uint64_t>(digits);
        return static_cast<int>(((d * (log10_2 >> half)) + ((d * (log10_2 & low)) >> half)) >> half);
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_LIMITS_DETAIL_DIGITS10_HPP
