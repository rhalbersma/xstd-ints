//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONSTANT_TIME_CASES_HPP
#define XSTD_INTS_CONSTANT_TIME_CASES_HPP

#include <xstd/ints/bit/detail/portable/block_compress.hpp> // block_compress_by_rank
#include <xstd/ints/bit/detail/portable/block_expand.hpp>   // block_expand_by_select, block_expand_by_walk
#include <xstd/ints/bit/detail/portable/block_reverse.hpp>  // block_reverse_by_swaps
#include <xstd/ints/bit/detail/portable/block_select.hpp>   // block_select_by_broadword
#include <array>                                            // array
#include <bit>                                              // popcount
#include <cstdint>                                          // uint64_t
#include <string_view>                                      // string_view

#ifdef __BMI2__
#include <immintrin.h> // _pdep_u64, _pext_u64
#endif

// Each kernel with the argument a caller would keep secret, and two ways to draw its inputs.
namespace constant_time {

struct inputs
{
        std::uint64_t a;
        std::uint64_t b;
};

// Named kernel.secret; fixed holds the secret at one value and random draws it, a public argument alike in both.
struct kernel_case
{
        std::string_view name;
        bool a_secret;
        bool b_secret;
        auto (*run)(inputs) noexcept -> std::uint64_t;
        auto (*fixed)(std::uint64_t, std::uint64_t) noexcept -> inputs;
        auto (*random)(std::uint64_t, std::uint64_t) noexcept -> inputs;
};

namespace portable = xstd::ints::detail::portable;

inline constexpr auto alternate = std::uint64_t{0x5555'5555'5555'5555};
inline constexpr auto all_ones  = ~std::uint64_t{0};

// A word with at least one one, and a rank below its population, as block_select_by_broadword requires.
[[nodiscard]] constexpr auto selectable(std::uint64_t x, std::uint64_t r) noexcept
        -> inputs
{
        x = x == 0 ? 1 : x;
        return {x, r % static_cast<std::uint64_t>(std::popcount(x))};
}

inline constexpr auto cases = std::array{
        kernel_case{
                "reverse_by_swaps.word",
                true,
                false,
                [](inputs in) noexcept -> std::uint64_t { return portable::block_reverse_by_swaps(in.a); },
                [](std::uint64_t, std::uint64_t) noexcept -> inputs { return {0, 0}; },
                [](std::uint64_t r, std::uint64_t) noexcept -> inputs { return {r, 0}; },
        },
        kernel_case{
                "select_by_broadword.dense",
                true,
                true,
                [](inputs in) noexcept -> std::uint64_t { return portable::block_select_by_broadword(in.a, static_cast<unsigned>(in.b)); },
                [](std::uint64_t, std::uint64_t s) noexcept -> inputs { return selectable(all_ones, s); },
                [](std::uint64_t r, std::uint64_t s) noexcept -> inputs { return selectable(r, s); },
        },
        kernel_case{
                "select_by_broadword.one_bit",
                true,
                true,
                [](inputs in) noexcept -> std::uint64_t { return portable::block_select_by_broadword(in.a, static_cast<unsigned>(in.b)); },
                [](std::uint64_t, std::uint64_t) noexcept -> inputs { return {std::uint64_t{1} << 63U, 0}; },
                [](std::uint64_t r, std::uint64_t s) noexcept -> inputs { return selectable(r, s); },
        },
        kernel_case{
                "expand_by_walk.source",
                true,
                false,
                [](inputs in) noexcept -> std::uint64_t { return portable::block_expand_by_walk(in.a, in.b); },
                [](std::uint64_t, std::uint64_t) noexcept -> inputs { return {0, alternate}; },
                [](std::uint64_t r, std::uint64_t) noexcept -> inputs { return {r, alternate}; },
        },
        kernel_case{
                "expand_by_walk.mask",
                false,
                true,
                [](inputs in) noexcept -> std::uint64_t { return portable::block_expand_by_walk(in.a, in.b); },
                [](std::uint64_t r, std::uint64_t) noexcept -> inputs { return {r, 1}; },
                [](std::uint64_t r, std::uint64_t s) noexcept -> inputs { return {r, s}; },
        },
        kernel_case{
                "compress_by_rank.source",
                true,
                false,
                [](inputs in) noexcept -> std::uint64_t { return portable::block_compress_by_rank(in.a, in.b); },
                [](std::uint64_t, std::uint64_t) noexcept -> inputs { return {0, all_ones}; },
                [](std::uint64_t r, std::uint64_t) noexcept -> inputs { return {r, all_ones}; },
        },
        kernel_case{
                "expand_by_select.source",
                true,
                false,
                [](inputs in) noexcept -> std::uint64_t { return portable::block_expand_by_select(in.a, in.b); },
                [](std::uint64_t, std::uint64_t) noexcept -> inputs { return {0, all_ones}; },
                [](std::uint64_t r, std::uint64_t) noexcept -> inputs { return {r, all_ones}; },
        },
#ifdef __BMI2__
        kernel_case{
                "pext.source",
                true,
                false,
                [](inputs in) noexcept -> std::uint64_t { return _pext_u64(in.a, in.b); },
                [](std::uint64_t, std::uint64_t) noexcept -> inputs { return {0, alternate}; },
                [](std::uint64_t r, std::uint64_t) noexcept -> inputs { return {r, alternate}; },
        },
        kernel_case{
                "pext.mask",
                false,
                true,
                [](inputs in) noexcept -> std::uint64_t { return _pext_u64(in.a, in.b); },
                [](std::uint64_t r, std::uint64_t) noexcept -> inputs { return {r, 1}; },
                [](std::uint64_t r, std::uint64_t s) noexcept -> inputs { return {r, s}; },
        },
        kernel_case{
                "pdep.mask",
                false,
                true,
                [](inputs in) noexcept -> std::uint64_t { return _pdep_u64(in.a, in.b); },
                [](std::uint64_t r, std::uint64_t) noexcept -> inputs { return {r, 1}; },
                [](std::uint64_t r, std::uint64_t s) noexcept -> inputs { return {r, s}; },
        },
#endif
};

} // namespace constant_time

#endif // XSTD_INTS_CONSTANT_TIME_CASES_HPP
