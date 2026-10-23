//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/abs.hpp>           // abs
#include <xstd/ints/limits.hpp>                // numeric_limits
#include <test/boost_test_print_log_value.hpp> // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>          // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>            // Boost.Test
#include <concepts>                            // same_as

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)
BOOST_AUTO_TEST_SUITE(Abs)

BOOST_AUTO_TEST_CASE_TEMPLATE(ReturnsTheMagnitude, T, test::exact_width_signed_integer_types)
{
        // T itself, where std::abs of a std::int8_t returns the promoted int.
        static_assert(std::same_as<decltype(xstd::abs(T{})), T>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{-2}), T{2});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{-1}), T{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{0}), T{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{+1}), T{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{+2}), T{2});

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::max()), limits::max());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(static_cast<T>(limits::min() + 1)), limits::max());
}

// The identity, and total where the signed case is not: min() is 0, its own magnitude.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsTheIdentityOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        static_assert(std::same_as<decltype(xstd::abs(T{})), T>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{0}), T{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{1}), T{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{2}), T{2});

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::min()), limits::min());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::max()), limits::max());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
