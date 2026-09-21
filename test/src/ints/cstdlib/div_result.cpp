//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div_result.hpp> // div_result
#include <test/exact_width_types.hpp>       // std_signed_types, std_unsigned_types
#include <boost/test/unit_test.hpp>         // Boost.Test
#include <concepts>                         // same_as
#include <cstddef>                          // size_t
#include <utility>                          // declval

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)
BOOST_AUTO_TEST_SUITE(DivResult)

// div_result{q, r} spells a div result at the argument's own width.
BOOST_AUTO_TEST_CASE(DeducesItsElementTypeFromTheArguments)
{
        static_assert(std::same_as<decltype(xstd::div_result{1, 2}), xstd::div_result<int>>);
        static_assert(std::same_as<decltype(xstd::div_result{1L, 2L}), xstd::div_result<long>>);
        static_assert(std::same_as<decltype(xstd::div_result{1LL, 2LL}), xstd::div_result<long long>>);

        // And at an unsigned width, div_result being open to every integer element type.
        static_assert(std::same_as<decltype(xstd::div_result{1U, 2U}), xstd::div_result<unsigned>>);
        static_assert(std::same_as<decltype(xstd::div_result{1UL, 2UL}), xstd::div_result<unsigned long>>);
        static_assert(std::same_as<decltype(xstd::div_result{1ULL, 2ULL}), xstd::div_result<unsigned long long>>);
        static_assert(std::same_as<decltype(xstd::div_result{1UZ, 2UZ}), xstd::div_result<std::size_t>>);

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(AggregateMembersAndStructuredBindings)
{
        constexpr auto result = xstd::div_result{.quotient = -3, .remainder = 1};
        static_assert(result.quotient == -3);
        static_assert(result.remainder == 1);
        auto const [quotient, remainder] = result;
        BOOST_CHECK(quotient == -3 and remainder == 1);
        static_assert(result == xstd::div_result{-3, 1});
}

// The computed exception specification is the element's; a written one would need keeping.
BOOST_AUTO_TEST_CASE_TEMPLATE(EqualityTracksTheElementType, T, test::std_signed_types)
{
        static_assert(noexcept(std::declval<xstd::div_result<T> const&>() == std::declval<xstd::div_result<T> const&>()) == noexcept(std::declval<T const&>() == std::declval<T const&>()));

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EqualityTracksTheUnsignedElementType, T, test::std_unsigned_types)
{
        static_assert(noexcept(std::declval<xstd::div_result<T> const&>() == std::declval<xstd::div_result<T> const&>()) == noexcept(std::declval<T const&>() == std::declval<T const&>()));

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
