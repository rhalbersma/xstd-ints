//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdlib/unsigned_abs.hpp>             // unsigned_abs
#include <xstd/ints/limits.hpp>                           // numeric_limits
#include <xstd/ints/type_traits/make_unsigned.hpp>        // make_unsigned_t
#include <test/boost_test_print_log_value.hpp>            // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>                     // exact_width_integer_types, exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>                       // Boost.Test
#include <concepts>                                       // same_as

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)
BOOST_AUTO_TEST_SUITE(UnsignedAbs)

// The MIN-boundary wraparound, at compile time: what distinguishes unsigned_abs from abs.
BOOST_AUTO_TEST_CASE_TEMPLATE(ReturnsTheMagnitudeIncludingAtMin, T, test::exact_width_signed_integer_types)
{
        using U = xstd::make_unsigned_t<T>;

        // The counterpart, where comparing with a U would convert a signed result and accept it.
        static_assert(std::same_as<decltype(xstd::unsigned_abs(T{})), U>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{-2}), U{2});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{0}), U{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{+2}), U{2});

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::min()), static_cast<U>(static_cast<U>(limits::max()) + U{1}));
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::max()), static_cast<U>(limits::max()));
}

// The identity, and its own counterpart, so that boundary is not a boundary here.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsTheIdentityOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        static_assert(std::same_as<xstd::make_unsigned_t<T>, T>);
        static_assert(std::same_as<decltype(xstd::unsigned_abs(T{})), T>);

        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{0}), T{0});
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(T{2}), T{2});

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::min()), limits::min());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::unsigned_abs(limits::max()), limits::max());
}

// noexcept exactly where the type's own operators are, which Abseil's 128-bit types do not declare.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsNoexceptWhereTheOperatorsAre, T, test::exact_width_integer_types)
{
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::unsigned_abs(T{})) == xstd::nothrow_const_operators<T>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
