//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_PORTABLE_COMPRESS_HPP
#define XSTD_INTS_BIT_DETAIL_PORTABLE_COMPRESS_HPP

#include <xstd/ints/bit/detail/bit_block.hpp> // bit_block
#include <bit>                                // popcount

namespace xstd::ints::detail::portable {

// Each selected bit of src lands at its rank among the mask's ones: one popcount per selected bit, none per mask bit.
[[nodiscard]] constexpr auto compress_by_rank(bit_block src, bit_block mask) noexcept
        -> bit_block
{
        auto dst = 0UZ;
        for (auto selected = src & mask; selected != 0UZ; selected &= selected - 1UZ) {
                auto const below_lowest = (selected - 1UZ) & ~selected;
                dst |= 1UZ << static_cast<unsigned>(std::popcount(below_lowest & mask));
        }
        return dst;
}

} // namespace xstd::ints::detail::portable

#endif // XSTD_INTS_BIT_DETAIL_PORTABLE_COMPRESS_HPP
