//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/detail/bit_limb.hpp>      // bit_limb
#include <xstd/ints/bit/detail/compress_limb.hpp> // compress_limb
#include <xstd/ints/bit/detail/expand_limb.hpp>   // expand_limb
#include <xstd/ints/bit/detail/intrinsic.hpp>     // IWYU pragma: keep; compress, expand, XSTD_INTS_HAS_FAST_BMI2
#include <xstd/ints/bit/detail/portable.hpp>      // compress_by_rank, expand_by_select, expand_by_walk
#include <test/bit_reference.hpp>                 // for_each_sweep_pair
#include <boost/test/unit_test.hpp>               // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK_EQUAL, BOOST_TEST_MESSAGE

namespace {

using xstd::ints::detail::bit_limb;
namespace portable = xstd::ints::detail::portable;

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Detail)
BOOST_AUTO_TEST_SUITE(Intrinsic)

// The run-time limb kernels match the portable ones, whether they run the instruction or not.
BOOST_AUTO_TEST_CASE(TheRunTimeKernelsAgreeWithThePortableOnes)
{
#ifdef XSTD_INTS_HAS_FAST_BMI2
        BOOST_TEST_MESSAGE("PEXT and PDEP are fast on this target: the run-time kernels are the instructions.");
#else
        BOOST_TEST_MESSAGE("PEXT and PDEP are not fast on this target: the run-time kernels are the portable ones.");
#endif
        auto mismatches = 0UZ;
        test::for_each_sweep_pair<bit_limb>([&](bit_limb x, bit_limb m) -> void {
                mismatches += xstd::ints::detail::compress_limb(x, m) == portable::compress_by_rank(x, m) ? 0UZ : 1UZ;
                mismatches += xstd::ints::detail::expand_limb(x, m) == portable::expand_by_walk(x, m) ? 0UZ : 1UZ;
                mismatches += xstd::ints::detail::expand_limb(x, m) == portable::expand_by_select(x, m) ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

#ifdef XSTD_INTS_HAS_FAST_BMI2

BOOST_AUTO_TEST_CASE(TheInstructionsAgreeWithThePortableKernels)
{
        namespace intrinsic = xstd::ints::detail::intrinsic;
        auto mismatches     = 0UZ;
        test::for_each_sweep_pair<bit_limb>([&](bit_limb x, bit_limb m) -> void {
                mismatches += intrinsic::compress(x, m) == portable::compress_by_rank(x, m) ? 0UZ : 1UZ;
                mismatches += intrinsic::expand(x, m) == portable::expand_by_walk(x, m) ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
