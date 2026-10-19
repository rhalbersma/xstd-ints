//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div.hpp>           // div
#include <xstd/ints/limits.hpp>                // numeric_limits
#include <test/boost_test_print_log_value.hpp> // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>          // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>            // Boost.Test

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)
BOOST_AUTO_TEST_SUITE(Div)

// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

BOOST_AUTO_TEST_CASE_TEMPLATE(TruncatesTowardZero, T, test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{+8}, T{+3})), (xstd::div_result<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{+8}, T{-3})), (xstd::div_result<T>{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-8}, T{+3})), (xstd::div_result<T>{-2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-8}, T{-3})), (xstd::div_result<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{+1}, T{+2})), (xstd::div_result<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{+1}, T{-2})), (xstd::div_result<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-1}, T{+2})), (xstd::div_result<T>{0, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-1}, T{-2})), (xstd::div_result<T>{0, -1}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TruncatesTowardZeroOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{8}, T{3})), (xstd::div_result<T>{2, 2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{1}, T{2})), (xstd::div_result<T>{0, 1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{0}, T{3})), (xstd::div_result<T>{0, 0}));

        // min() and static_cast<T>(-1) together, which unsigned is just 0 and max().
        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(limits::min(), limits::max())), (xstd::div_result<T>{0, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(limits::max(), limits::max())), (xstd::div_result<T>{1, 0}));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
