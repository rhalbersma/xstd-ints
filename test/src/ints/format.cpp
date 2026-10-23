//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div_result.hpp> // div_result
#include <xstd/ints/format.hpp>             // IWYU pragma: keep; formatter<div_result>
#include <boost/test/unit_test.hpp>         // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <format>                           // format

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Format)

// One formatter behind this door, so no invariant spans two; that it arrives is what the umbrella answers for.
BOOST_AUTO_TEST_CASE(TheFormatterArrivesThroughTheUmbrella)
{
        BOOST_CHECK_EQUAL(std::format("{}", xstd::div_result<int>{1, -2}), "(1, -2)");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
