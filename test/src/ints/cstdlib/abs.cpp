//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/abs.hpp>             // abs
#include <xstd/ints/limits.hpp>                  // numeric_limits
#include <xstd/ints/type_traits/is_unsigned.hpp> // is_unsigned_v
#include <xstd/ints/type_traits/promoted.hpp>    // promoted_t
#include <test/boost_test_print_log_value.hpp>   // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <test/constexpr_check.hpp>              // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>            // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>              // Boost.Test
#include <concepts>                              // same_as

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

// Named, not a bare requires-expression: outside a template an invalid requirement is ill-formed, not false.
template<class T>
concept has_abs = requires (T x) { xstd::abs(x); };

// [c.math.abs]/2: an unsigned type that promotes to int takes the identity there, and a wider one is refused.
BOOST_AUTO_TEST_CASE_TEMPLATE(TakesUnsignedTypesOnlyThroughInt, T, test::exact_width_unsigned_integer_types)
{
        using P = xstd::promoted_t<T>;
        if constexpr (xstd::is_unsigned_v<P>) {
                // abs(0u), the mistake LWG 2192 made ill-formed: unsigned_abs is what such a call meant.
                static_assert(not has_abs<T>);
        } else {
                static_assert(std::same_as<decltype(xstd::abs(T{})), P>);
                using limits = xstd::numeric_limits<T>;
                XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::min()), P{0});
                XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(T{2}), P{2});
                XSTD_CONSTEXPR_CHECK_EQUAL(xstd::abs(limits::max()), static_cast<P>(limits::max()));
        }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
