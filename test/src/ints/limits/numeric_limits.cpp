//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK_EQUAL
#include <boost/test/unit_test.hpp>            // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                            // derived_from
#include <type_traits>                         // is_empty_v
#include <limits>                              // numeric_limits

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Limits)
BOOST_AUTO_TEST_SUITE(NumericLimits)

BOOST_AUTO_TEST_CASE(DelegatesToTheStandardDomain)
{
        static_assert(std::derived_from<xstd::numeric_limits<int>, std::numeric_limits<int>>);
        static_assert(std::derived_from<xstd::numeric_limits<int const>, std::numeric_limits<int>>);
        static_assert(std::derived_from<xstd::numeric_limits<unsigned volatile>, std::numeric_limits<unsigned>>);
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::numeric_limits<int>::min(), std::numeric_limits<int>::min());
        XSTD_CONSTEXPR_CHECK_EQUAL(xstd::numeric_limits<int>::max(), std::numeric_limits<int>::max());
}

// A named concept, not a bare requires-expression: naming a member of an empty trait is ill-formed.
template<class T>
concept has_limits = requires { xstd::numeric_limits<T>::is_specialized; };

BOOST_AUTO_TEST_CASE(SaysNothingOutsideThatDomain)
{
        static_assert(has_limits<int>);
        static_assert(not has_limits<void>);
        static_assert(not has_limits<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        static_assert(not has_limits<int[3]>);
        static_assert(not has_limits<void()>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): routed through the cv specialization, which must be complete
        static_assert(not has_limits<int const[3]>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): as above
        static_assert(std::is_empty_v<xstd::numeric_limits<int[3]>>);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
