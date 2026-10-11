//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_PORTABLE_COMPRESS_HPP
#define XSTD_INTS_DETAIL_PORTABLE_COMPRESS_HPP

#include <xstd/ints/detail/bit_limb.hpp> // bit_limb
#include <bit>                           // popcount

namespace xstd::ints::detail::portable {

// Each selected bit of x lands at its rank among the mask's ones: one popcount per selected bit, none per mask bit.
[[nodiscard]] constexpr auto compress_by_rank(bit_limb x, bit_limb m) noexcept
        -> bit_limb
{
        auto result = bit_limb{0};
        for (auto selected = x & m; selected != bit_limb{0}; selected &= selected - 1U) {
                auto const below_lowest = (selected - 1U) & ~selected;
                result |= bit_limb{1} << static_cast<unsigned>(std::popcount(below_lowest & m));
        }
        return result;
}

} // namespace xstd::ints::detail::portable

#endif // XSTD_INTS_DETAIL_PORTABLE_COMPRESS_HPP
