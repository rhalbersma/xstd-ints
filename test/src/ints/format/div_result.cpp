//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint.hpp>            // int128
#include <xstd/ints/cstdlib/div_result.hpp> // div_result
#include <xstd/ints/format/div_result.hpp>  // IWYU pragma: keep; formatter<div_result>
#include <test/exact_width_types.hpp>       // exact_width_signed_integer_types
#include <boost/test/unit_test.hpp>         // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <format>                           // format
#include <limits>                           // numeric_limits
#include <string_view>                      // wstring_view

#ifdef __cpp_lib_constexpr_format
#include <test/constexpr_check.hpp> // XSTD_CONSTEXPR_CHECK_EQUAL
#endif

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Format)
BOOST_AUTO_TEST_SUITE(DivResult)

// Which specialization runs is the library's and the element type's, so no test names one.

// The formatting checker, whose static_assert half arrives only with P3391.
#if defined(__cpp_lib_constexpr_format) && __cpp_lib_constexpr_format >= 202511L
#define XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(a, b) XSTD_CONSTEXPR_CHECK_EQUAL((a), (b))
#else
#define XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(a, b) BOOST_CHECK_EQUAL((a), (b))
#endif

BOOST_AUTO_TEST_CASE_TEMPLATE(Formatter, T, test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_FORMAT_CHECK_EQUAL(std::format("{}", xstd::div_result<T>{1, -2}), "(1, -2)");
}

// Rendered by xstd where the library has no 128-bit formatter, so the value must come out.
BOOST_AUTO_TEST_CASE(Boundaries)
{
        BOOST_CHECK_EQUAL(std::format("{}", xstd::div_result<xstd::int128>{std::numeric_limits<xstd::int128>::min(), 7}), "(-170141183460469231731687303715884105728, 7)");
}

// Fill, alignment and width stay the base's; this renders "(quotient, remainder)" and hands it over.
BOOST_AUTO_TEST_CASE(FillAndAlign)
{
        BOOST_CHECK_EQUAL(std::format("{:>12}", xstd::div_result<int>{1, -2}), "     (1, -2)");
        BOOST_CHECK_EQUAL(std::format("{:*^12}", xstd::div_result<int>{1, -2}), "**(1, -2)***");
}

BOOST_AUTO_TEST_CASE(WideFormatter)
{
        BOOST_CHECK(std::format(L"{}", xstd::div_result<int>{1, -2}) == std::wstring_view{L"(1, -2)"});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
