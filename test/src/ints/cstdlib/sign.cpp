//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdlib/sign.hpp>                     // sign
#include <xstd/ints/limits.hpp>                           // numeric_limits
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>                     // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>                       // Boost.Test
#include <concepts>                                       // same_as

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)
BOOST_AUTO_TEST_SUITE(Sign)

BOOST_AUTO_TEST_CASE_TEMPLATE(IsMinusOneZeroOrPlusOne, T, test::exact_width_signed_integer_types)
{
        // An int whatever the width, where a comparison with an int literal would accept any integer result.
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{-2}), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{-1}), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{0}), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{+1}), +1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{+2}), +1);

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(limits::min()), -1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(limits::max()), +1);
}

// Two of the three values, -1 being unreachable: nothing lies below zero.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsZeroOrPlusOneOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        static_assert(std::same_as<decltype(xstd::sign(T{})), int>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{0}), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{1}), +1);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(T{2}), +1);

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(limits::min()), 0);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::sign(limits::max()), +1);
}

// noexcept exactly where the type's own operators are, which Abseil's 128-bit types do not declare.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsNoexceptWhereTheOperatorsAre, T, test::exact_width_integer_types)
{
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::sign(T{})) == xstd::nothrow_const_operators<T>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
