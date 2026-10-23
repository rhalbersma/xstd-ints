//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/abs.hpp>           // abs
#include <xstd/ints/limits.hpp>                // numeric_limits
#include <xstd/ints/type_traits/promoted.hpp>  // promoted_t
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
        // The promoted type, as std::abs gives it: an int for a std::int8_t.
        using P = xstd::promoted_t<T>;
        static_assert(std::same_as<decltype(xstd::abs(T{})), P>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{-2}), P{2});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{-1}), P{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{0}), P{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{+1}), P{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{+2}), P{2});

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::max()), static_cast<P>(limits::max()));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(static_cast<T>(limits::min() + 1)), static_cast<P>(limits::max()));
        // Below int the minimum's magnitude fits the promoted type, so it is in the domain, as std::abs(SCHAR_MIN) is.
        if constexpr (not std::same_as<P, T>) {
                XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::min()), static_cast<P>(-static_cast<P>(limits::min())));
        }
}

// The identity, and total where the signed case is not: min() is 0, its own magnitude.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsTheIdentityOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        // The promoted type here too, a signed int for a std::uint8_t.
        using P = xstd::promoted_t<T>;
        static_assert(std::same_as<decltype(xstd::abs(T{})), P>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{0}), P{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{1}), P{1});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{2}), P{2});

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::min()), static_cast<P>(limits::min()));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::max()), static_cast<P>(limits::max()));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
