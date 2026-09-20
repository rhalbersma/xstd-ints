//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/signed_integer.hpp> // signed_integer
#include <test/exact_width_types.hpp>            // exact-width integer types
#include <boost/test/unit_test.hpp>              // Boost.Test

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(SignedInteger)

BOOST_AUTO_TEST_CASE_TEMPLATE(AdmitsTheExactWidthTypesThroughCv, T, test::exact_width_signed_integer_types)
{
        static_assert(xstd::signed_integer<T>);
        static_assert(xstd::signed_integer<T const>);
        static_assert(xstd::signed_integer<T volatile>);
        static_assert(xstd::signed_integer<T const volatile>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedIsNotSigned, T, test::exact_width_unsigned_integer_types)
{
        static_assert(not xstd::signed_integer<T>);
        BOOST_CHECK(true);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
