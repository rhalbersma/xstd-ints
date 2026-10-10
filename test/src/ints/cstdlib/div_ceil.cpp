//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div_ceil.hpp>      // div_ceil
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/boost_test_print_log_value.hpp> // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <test/exact_width_types.hpp>          // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>            // Boost.Test
#include <initializer_list>                    // initializer_list

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)
BOOST_AUTO_TEST_SUITE(DivCeil)

BOOST_AUTO_TEST_CASE_TEMPLATE(RoundsTowardPositiveInfinity, T, test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{+8}, T{+3})), (xstd::div_result<T>{+3, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{+8}, T{-3})), (xstd::div_result<T>{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{-8}, T{+3})), (xstd::div_result<T>{-2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{-8}, T{-3})), (xstd::div_result<T>{+3, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{+1}, T{+2})), (xstd::div_result<T>{+1, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{+1}, T{-2})), (xstd::div_result<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{-1}, T{+2})), (xstd::div_result<T>{0, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{-1}, T{-2})), (xstd::div_result<T>{+1, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{+9}, T{+3})), (xstd::div_result<T>{+3, 0}));
}

// Unsigned, an inexact quotient still rounds up, and the negative remainder it leaves is the type's modular one.
BOOST_AUTO_TEST_CASE_TEMPLATE(RoundsTowardPositiveInfinityOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        constexpr auto minus = [](T x) -> T { return static_cast<T>(T{0} - x); };
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{8}, T{3})), (xstd::div_result<T>{3, minus(T{1})}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{1}, T{2})), (xstd::div_result<T>{1, minus(T{1})}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{40}, T{17})), (xstd::div_result<T>{3, minus(T{11})}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{9}, T{3})), (xstd::div_result<T>{3, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div_ceil(T{0}, T{3})), (xstd::div_result<T>{0, 0}));
}

// numerator == denominator * quotient + remainder, in the type's own arithmetic, for every sign of either.
BOOST_AUTO_TEST_CASE_TEMPLATE(KeepsTheDivisionIdentity, T, test::exact_width_signed_integer_types)
{
        for (auto const numer : {T{-9}, T{-8}, T{-1}, T{0}, T{1}, T{8}, T{9}}) {
                for (auto const denom : {T{-3}, T{-2}, T{2}, T{3}}) {
                        auto const [quotient, remainder] = xstd::div_ceil(numer, denom);
                        BOOST_CHECK(numer == static_cast<T>(denom * quotient + remainder));
                }
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(KeepsTheDivisionIdentityOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        for (auto const numer : {T{0}, T{1}, T{8}, T{9}, T{40}}) {
                for (auto const denom : {T{1}, T{2}, T{3}, T{17}}) {
                        auto const [quotient, remainder] = xstd::div_ceil(numer, denom);
                        BOOST_CHECK(numer == static_cast<T>(denom * quotient + remainder));
                }
        }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
