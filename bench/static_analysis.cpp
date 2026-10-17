//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/detail/intrinsic/block_reverse.hpp> // IWYU pragma: keep; block_reverse and the macros naming which one it is
#include <xstd/ints/bit/detail/portable/block_compress.hpp> // block_compress_by_rank
#include <xstd/ints/bit/detail/portable/block_expand.hpp>   // block_expand_by_select, block_expand_by_walk
#include <xstd/ints/bit/detail/portable/block_reverse.hpp>  // block_reverse_by_swaps
#include <xstd/ints/bit/detail/portable/block_select.hpp>   // block_select_by_broadword
#include <alternatives.hpp>                                 // compress_by_walk_with_if, compress_by_parallel_suffix, expand_by_walk_with_if, expand_by_parallel_suffix
#include <zp7.hpp>                                          // zp7_by_clmul, zp7_by_shifts
#include <bit>                                              // countr_zero
#include <cstddef>                                          // size_t

#ifdef __BMI2__
#include <immintrin.h> // _pdep_u64, _pext_u64
#endif

// Each kernel as a function of its own with an unmangled name, compiled to assembly for llvm-mca to read.
namespace portable = xstd::ints::detail::portable;

extern "C" {

[[nodiscard]] auto reverse_by_swaps(std::size_t x) noexcept
        -> std::size_t
{
        return portable::block_reverse_by_swaps(x);
}

#if defined(XSTD_INTS_HAS_BUILTIN_BITREVERSE) or defined(XSTD_INTS_HAS_RBIT) or defined(XSTD_INTS_HAS_GFNI)
[[nodiscard]] auto reverse_intrinsic(std::size_t x) noexcept
        -> std::size_t
{
        return xstd::ints::detail::intrinsic::block_reverse(x);
}
#endif

[[nodiscard]] auto compress_by_rank(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return portable::block_compress_by_rank(src, mask);
}

[[nodiscard]] auto compress_by_walk_with_if(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return bench::compress_by_walk_with_if(src, mask);
}

[[nodiscard]] auto compress_by_parallel_suffix(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return bench::compress_by_parallel_suffix(src, mask);
}

[[nodiscard]] auto expand_by_walk(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return portable::block_expand_by_walk(src, mask);
}

[[nodiscard]] auto expand_by_select(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return portable::block_expand_by_select(src, mask);
}

[[nodiscard]] auto expand_by_walk_with_if(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return bench::expand_by_walk_with_if(src, mask);
}

[[nodiscard]] auto expand_by_parallel_suffix(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return bench::expand_by_parallel_suffix(src, mask);
}

[[nodiscard]] auto select_by_broadword(std::size_t x, unsigned k) noexcept
        -> unsigned
{
        return portable::block_select_by_broadword(x, k);
}

[[nodiscard]] auto compress_by_zp7(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return bench::zp7_by_shifts::zp7_pext_64(src, mask);
}

[[nodiscard]] auto expand_by_zp7(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return bench::zp7_by_shifts::zp7_pdep_64(src, mask);
}

#ifdef __PCLMUL__
[[nodiscard]] auto compress_by_zp7_clmul(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return bench::zp7_by_clmul::zp7_pext_64(src, mask);
}

[[nodiscard]] auto expand_by_zp7_clmul(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return bench::zp7_by_clmul::zp7_pdep_64(src, mask);
}
#endif

#ifdef __BMI2__
[[nodiscard]] auto compress_pext(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return _pext_u64(src, mask);
}

[[nodiscard]] auto expand_pdep(std::size_t src, std::size_t mask) noexcept
        -> std::size_t
{
        return _pdep_u64(src, mask);
}

[[nodiscard]] auto select_pdep(std::size_t x, unsigned k) noexcept
        -> unsigned
{
        return static_cast<unsigned>(std::countr_zero(_pdep_u64(1ULL << k, x)));
}
#endif

} // extern "C"
