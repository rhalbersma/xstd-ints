//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_BLOCK_REVERSE_HPP
#define XSTD_INTS_BIT_DETAIL_BLOCK_REVERSE_HPP

#include <xstd/ints/bit/detail/intrinsic/block_reverse.hpp> // IWYU pragma: keep; block_reverse and XSTD_INTS_HAS_BUILTIN_BITREVERSE, where the compiler or the target mirrors a block
#include <xstd/ints/bit/detail/portable/block_reverse.hpp>  // IWYU pragma: keep; block_reverse_by_swaps, a constant expression on every target
#include <cstddef>                                          // size_t

namespace xstd::ints::detail {

// The compiler's builtin wherever it has one, in a constant expression too; else RBIT or GFNI at run time.
[[nodiscard]] constexpr auto block_reverse(std::size_t x) noexcept
        -> std::size_t
{
#ifdef XSTD_INTS_HAS_BUILTIN_BITREVERSE
        return intrinsic::block_reverse(x);
#else
#if defined(XSTD_INTS_HAS_RBIT) or defined(XSTD_INTS_HAS_GFNI)
        if not consteval {
                return intrinsic::block_reverse(x);
        }
#endif
        return portable::block_reverse_by_swaps(x);
#endif
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_BLOCK_REVERSE_HPP
