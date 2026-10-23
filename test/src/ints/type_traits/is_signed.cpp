//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/type_traits/is_signed.hpp> // is_signed
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>          // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>            // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <type_traits>                         // is_signed_v, is_unsigned_v

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsSigned)

// An incomplete class type is answered, not hard-errored, as the widened traits are.
BOOST_AUTO_TEST_CASE(AnswersForIncompleteTypes)
{
        XSTD_CONSTEXPR_CHECK(not xstd::is_signed_v<struct never_defined>);
}

BOOST_AUTO_TEST_CASE(AgreesWithStd)
{
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_v<int> == std::is_signed_v<int>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_v<unsigned> == std::is_signed_v<unsigned>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_v<double> == std::is_signed_v<double>);
        XSTD_CONSTEXPR_CHECK(xstd::is_signed_v<void> == std::is_signed_v<void>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ExactWidthIntegers, T, test::exact_width_signed_integer_types)
{
        static_assert(xstd::is_signed_v<T>);
        static_assert(xstd::is_signed_v<T const>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(OppositeSignExactWidthIntegers, T, test::exact_width_unsigned_integer_types)
{
        static_assert(not xstd::is_signed_v<T>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
