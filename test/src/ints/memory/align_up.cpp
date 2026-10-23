//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory/align_up.hpp>    // align_up
#include <xstd/ints/concepts/alignable.hpp> // nothrow_alignable
#include <test/constexpr_check.hpp>         // XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>         // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <concepts>                         // same_as
#include <cstddef>                          // size_t
#include <cstdint>                          // uint8_t, uint16_t
#include <limits>                           // numeric_limits

#if __has_include(<absl/numeric/int128.h>)
#define TEST_HAS_ABSL_INT128
#include <absl/numeric/int128.h> // uint128
#endif

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Memory)
BOOST_AUTO_TEST_SUITE(AlignUp)

BOOST_AUTO_TEST_CASE(RoundsUpToTheNextMultiple)
{
        // clang-format off
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up( 0UZ, 8),  0UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up( 1UZ, 8),  8UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up( 8UZ, 8),  8UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up( 9UZ, 8), 16UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(64UZ, 8), 64UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(65UZ, 8), 72UZ);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(std::numeric_limits<std::size_t>::max() - 7, 8), std::numeric_limits<std::size_t>::max() - 7);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(std::numeric_limits<std::size_t>::max() - 8, 8), std::numeric_limits<std::size_t>::max() - 7);
        // clang-format on
}

// The narrow types stay themselves, where the operands alone would promote.
BOOST_AUTO_TEST_CASE(KeepsTheNarrowOperandType)
{
        static_assert(std::same_as<decltype(xstd::align_up(std::uint8_t{}, 4)), std::uint8_t>);
        static_assert(std::same_as<decltype(xstd::align_up(std::uint16_t{}, 4)), std::uint16_t>);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::align_up(std::uint8_t{5}, 4), std::uint8_t{8});
}

// The exception specification is the concept, so Abseil's uint128, which declares no noexcept, gets none.
BOOST_AUTO_TEST_CASE(IsNoexceptExactlyWhereTheTypeIsNothrowAlignable)
{
        static_assert(noexcept(xstd::align_up(std::size_t{}, 8)));
        static_assert(noexcept(xstd::align_up(std::uint8_t{}, 8)));
#ifdef TEST_HAS_ABSL_INT128
        static_assert(not noexcept(xstd::align_up(absl::uint128{}, 8)));
        static_assert(not xstd::nothrow_alignable<absl::uint128>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
