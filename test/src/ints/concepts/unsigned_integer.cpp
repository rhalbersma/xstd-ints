//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/concepts/integer.hpp>          // integer
#include <test/exact_width_types.hpp>              // exact-width integer types
#include <boost/test/unit_test.hpp>                // Boost.Test

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(UnsignedInteger)

BOOST_AUTO_TEST_CASE_TEMPLATE(AdmitsTheExactWidthTypesThroughCv, T, test::exact_width_unsigned_integer_types)
{
        static_assert(xstd::unsigned_integer<T>);
        static_assert(xstd::unsigned_integer<T const>);
        static_assert(xstd::unsigned_integer<T volatile>);
        static_assert(xstd::unsigned_integer<T const volatile>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SignedIsNotUnsigned, T, test::exact_width_signed_integer_types)
{
        static_assert(not xstd::unsigned_integer<T>);
        BOOST_CHECK(true);
}

namespace {

[[nodiscard]] constexpr auto refinement(xstd::integer auto)
        -> bool
{
        return false;
}

[[nodiscard]] constexpr auto refinement(xstd::unsigned_integer auto)
        -> bool
{
        return true;
}

} // namespace

// It subsumes integer, so an overload on it is chosen over one on integer for its models, and only for them.
BOOST_AUTO_TEST_CASE(SubsumesInteger)
{
        static_assert(refinement(1U));
        static_assert(not refinement(-1));
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
