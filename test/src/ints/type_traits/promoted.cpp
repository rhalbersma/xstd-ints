//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/type_traits/promoted.hpp> // promoted_t
#include <xstd/ints/cstdint.hpp>              // XSTD_HAS_BIT_INT, XSTD_HAS_INT128, bit_int, bit_uint, uint128
#include <boost/test/unit_test.hpp>           // Boost.Test
#include <concepts>                           // same_as
#include <type_traits>                        // type_identity

#if __has_include(<absl/numeric/int128.h>)
#define TEST_HAS_ABSL_INT128
#include <absl/numeric/int128.h> // uint128
#endif

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(Promoted)

BOOST_AUTO_TEST_CASE(MatchesTheIntegralPromotions)
{
        // Narrower than int promotes; int and wider are already their own.
        static_assert(std::same_as<xstd::promoted_t<char>, int>);
        static_assert(std::same_as<xstd::promoted_t<signed char>, int>);
        static_assert(std::same_as<xstd::promoted_t<unsigned char>, int>);
        static_assert(std::same_as<xstd::promoted_t<short>, int>);
        static_assert(std::same_as<xstd::promoted_t<int>, int>);
        static_assert(std::same_as<xstd::promoted_t<unsigned int>, unsigned int>);
        static_assert(std::same_as<xstd::promoted_t<long>, long>);
        static_assert(std::same_as<xstd::promoted_t<unsigned long>, unsigned long>);
        static_assert(std::same_as<xstd::promoted_t<long long>, long long>);
        static_assert(std::same_as<xstd::promoted_t<unsigned long long>, unsigned long long>);

        // Which of int and unsigned a narrow type promotes to is the platform's call, so assert only that it is one.
        static_assert(std::same_as<xstd::promoted_t<unsigned short>, int> or std::same_as<xstd::promoted_t<unsigned short>, unsigned int>);
        static_assert(std::same_as<xstd::promoted_t<char32_t>, int> or std::same_as<xstd::promoted_t<char32_t>, unsigned int>);

        // bool and a cv-qualified narrow type promote as their operands do.
        static_assert(std::same_as<xstd::promoted_t<bool>, int>);
        static_assert(std::same_as<xstd::promoted_t<short const>, int>);

        // A non-integral type is its own, and is never asked for a unary operator+.
        static_assert(std::same_as<xstd::promoted_t<std::type_identity<void>>, std::type_identity<void>>);
        BOOST_CHECK(true);
}

// An integer-class type and a bit-precise integer are exempt from promotion, so their operators yield themselves.
BOOST_AUTO_TEST_CASE(LeavesTheWideTypesAlone)
{
#ifdef XSTD_HAS_INT128
        static_assert(std::same_as<xstd::promoted_t<xstd::uint128>, xstd::uint128>);
#endif
#ifdef XSTD_HAS_BIT_INT
        static_assert(std::same_as<xstd::promoted_t<xstd::bit_uint<17>>, xstd::bit_uint<17>>);
        static_assert(std::same_as<xstd::promoted_t<xstd::bit_int<9>>, xstd::bit_int<9>>);
#endif
#ifdef TEST_HAS_ABSL_INT128
        static_assert(std::same_as<xstd::promoted_t<absl::uint128>, absl::uint128>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
