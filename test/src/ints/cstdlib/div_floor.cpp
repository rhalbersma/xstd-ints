//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdlib/div_floor.hpp>                // div_floor
#include <test/boost_test_print_log_value.hpp>            // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>                     // exact_width_integer_types, exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>                       // Boost.Test

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)
BOOST_AUTO_TEST_SUITE(DivFloor)

BOOST_AUTO_TEST_CASE_TEMPLATE(RoundsTowardNegativeInfinity, T, test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{+8}, T{+3})), (xstd::div_result<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{+8}, T{-3})), (xstd::div_result<T>{-3, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{-8}, T{+3})), (xstd::div_result<T>{-3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{-8}, T{-3})), (xstd::div_result<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{+1}, T{+2})), (xstd::div_result<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{+1}, T{-2})), (xstd::div_result<T>{-1, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{-1}, T{+2})), (xstd::div_result<T>{-1, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{-1}, T{-2})), (xstd::div_result<T>{0, -1}));
}

// Unsigned, the three conventions coincide: these are truncated division's own results.
BOOST_AUTO_TEST_CASE_TEMPLATE(RoundsTowardNegativeInfinityOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{8}, T{3})), (xstd::div_result<T>{2, 2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{1}, T{2})), (xstd::div_result<T>{0, 1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_floor(T{0}, T{3})), (xstd::div_result<T>{0, 0}));
}

// noexcept exactly where the type's own operators are, which Abseil's 128-bit types do not declare.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsNoexceptWhereTheOperatorsAre, T, test::exact_width_integer_types)
{
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div_floor(T{1}, T{1})) == xstd::nothrow_const_operators<T>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
