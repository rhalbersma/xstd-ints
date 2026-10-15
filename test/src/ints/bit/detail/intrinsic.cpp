//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/detail/block_compress.hpp>          // block_compress
#include <xstd/ints/bit/detail/block_expand.hpp>            // block_expand
#include <xstd/ints/bit/detail/block_reverse.hpp>           // block_reverse
#include <xstd/ints/bit/detail/block_select.hpp>            // block_select
#include <xstd/ints/bit/detail/intrinsic/block_reverse.hpp> // IWYU pragma: keep; block_reverse and XSTD_INTS_HAS_BUILTIN_BITREVERSE, where the compiler or the target mirrors a block
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp>          // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/bit/detail/intrinsic/gfni.hpp>          // IWYU pragma: keep; XSTD_INTS_HAS_GFNI, read only by the preprocessor
#include <xstd/ints/bit/detail/intrinsic/rbit.hpp>          // IWYU pragma: keep; XSTD_INTS_HAS_RBIT, read only by the preprocessor
#include <xstd/ints/bit/detail/portable/block_compress.hpp> // block_compress_by_rank
#include <xstd/ints/bit/detail/portable/block_expand.hpp>   // block_expand_by_select, block_expand_by_walk
#include <xstd/ints/bit/detail/portable/block_reverse.hpp>  // block_reverse_by_swaps
#include <xstd/ints/bit/detail/portable/block_select.hpp>   // block_select_by_broadword
#include <test/bit/oracle.hpp>                              // IWYU pragma: keep; oracle_bit_reverse, where the compiler or the target mirrors a block
#include <test/bit/permutation_properties.hpp>              // IWYU pragma: keep; intrinsics_agree, where PEXT and PDEP are fast
#include <test/bit/select_properties.hpp>                   // IWYU pragma: keep; intrinsic_select_agrees, where PDEP is fast
#include <test/bit/sweep.hpp>                               // for_each_sweep_pair, next_pseudo_random
#include <boost/test/unit_test.hpp>                         // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK_EQUAL, BOOST_TEST_MESSAGE
#include <bit>                                              // popcount
#include <cstddef>                                          // size_t
#include <cstdint>                                          // uint64_t
#include <ranges>                                           // iota

namespace {

namespace portable = xstd::ints::detail::portable;

// The sweep's pairs, then a reproducible sample of uniformly random ones.
template<class F>
auto for_each_pair(F f)
        -> void
{
        test::for_each_sweep_pair<std::size_t>(f);
        auto state = std::uint64_t{0x2545'f491'4f6c'dd1dU};
        for ([[maybe_unused]] auto const sample : std::views::iota(0UZ, 1UZ << 16U)) {
                auto const src = test::next_pseudo_random<std::size_t>(state);
                f(src, test::next_pseudo_random<std::size_t>(state));
        }
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Detail)
BOOST_AUTO_TEST_SUITE(Intrinsic)

// The run-time block kernels match the portable ones, whether they run the instruction or not.
BOOST_AUTO_TEST_CASE(TheRunTimeKernelsAgreeWithThePortableOnes)
{
#ifdef XSTD_INTS_HAS_FAST_BMI2
        BOOST_TEST_MESSAGE("PEXT and PDEP are fast on this target: the run-time kernels are the instructions.");
#else
        BOOST_TEST_MESSAGE("PEXT and PDEP are not fast on this target: the run-time kernels are the portable ones.");
#endif
        auto mismatches = 0UZ;
        for_each_pair([&](std::size_t x, std::size_t m) -> void {
                mismatches += xstd::ints::detail::block_compress(x, m) == portable::block_compress_by_rank(x, m) ? 0UZ : 1UZ;
                mismatches += xstd::ints::detail::block_expand(x, m) == portable::block_expand_by_walk(x, m) ? 0UZ : 1UZ;
                mismatches += xstd::ints::detail::block_expand(x, m) == portable::block_expand_by_select(x, m) ? 0UZ : 1UZ;
                for (auto const k : std::views::iota(0U, static_cast<unsigned>(std::popcount(m)))) {
                        mismatches += xstd::ints::detail::block_select(m, k) == portable::block_select_by_broadword(m, k) ? 0UZ : 1UZ;
                }
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// The run-time reversal matches the portable kernel, whether it is the builtin, an instruction or neither.
BOOST_AUTO_TEST_CASE(TheRunTimeReversalAgreesWithThePortableKernel)
{
#ifdef XSTD_INTS_HAS_BUILTIN_BITREVERSE
        BOOST_TEST_MESSAGE("The compiler has the bit reversal builtins: the run-time kernel is the builtin.");
#elifdef XSTD_INTS_HAS_RBIT
        BOOST_TEST_MESSAGE("The target has RBIT: the run-time kernel is the instruction.");
#elifdef XSTD_INTS_HAS_GFNI
        BOOST_TEST_MESSAGE("The target has GFNI: the run-time kernel mirrors every byte by one affine transform.");
#else
        BOOST_TEST_MESSAGE("The compiler has no bit reversal builtins: the run-time kernel is the portable one.");
#endif
        auto mismatches = 0UZ;
        for_each_pair([&](std::size_t x, std::size_t m) -> void {
                mismatches += xstd::ints::detail::block_reverse(x) == portable::block_reverse_by_swaps(x) ? 0UZ : 1UZ;
                mismatches += xstd::ints::detail::block_reverse(m) == portable::block_reverse_by_swaps(m) ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

#if defined(XSTD_INTS_HAS_BUILTIN_BITREVERSE) or defined(XSTD_INTS_HAS_RBIT) or defined(XSTD_INTS_HAS_GFNI)

// Every overload at its own width, the 32-bit one included, against the bit-at-a-time oracle.
BOOST_AUTO_TEST_CASE(TheIntrinsicsReverseAsTheOracle)
{
        auto const reverses = []<class U>(U x) -> bool { return xstd::ints::detail::intrinsic::block_reverse(x) == test::oracle_bit_reverse(x); };
#ifdef XSTD_INTS_HAS_BUILTIN_BITREVERSE
        static_assert(reverses(0x0000'1234U) and reverses(0x0000'1234UL) and reverses(0x0000'1234ULL));
#endif
        auto mismatches = 0UZ;
        for_each_pair([&](std::size_t x, std::size_t m) -> void {
                mismatches += reverses(static_cast<unsigned int>(x)) and reverses(static_cast<unsigned long>(x)) and reverses(static_cast<unsigned long long>(m)) ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

#endif

#ifdef XSTD_INTS_HAS_FAST_BMI2

// Every overload at its own width, the 32-bit one included, against the portable kernels and its own inverse.
BOOST_AUTO_TEST_CASE(TheInstructionsAgreeWithThePortableKernels)
{
        auto mismatches = 0UZ;
        for_each_pair([&](std::size_t x, std::size_t m) -> void { mismatches += test::intrinsics_agree(x, m) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// A lone bit deposited by PDEP selects the one of its rank, as the portable kernel does, at every rank of the block.
BOOST_AUTO_TEST_CASE(TheInstructionSelectsAsThePortableKernel)
{
        auto mismatches = 0UZ;
        for_each_pair([&](std::size_t x, std::size_t m) -> void { mismatches += test::intrinsic_select_agrees(x) and test::intrinsic_select_agrees(m) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
