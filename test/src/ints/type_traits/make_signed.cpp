//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/type_traits/make_signed.hpp> // make_signed
#include <xstd/ints/concepts/signed_integer.hpp> // signed_integer
#include <test/constexpr_check.hpp>              // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>            // exact-width integer types
#include <boost/test/unit_test.hpp>              // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <concepts>                              // same_as
#include <cstdint>                               // int8_t, uint8_t
#include <type_traits>                           // make_signed_t

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(MakeSigned)

enum class color : unsigned
{
        red = 1
};

// A named concept: a non-dependent invalid operand is a hard error on GCC.
template<class T>
concept has_make_signed = requires { typename xstd::make_signed_t<T>; };

BOOST_AUTO_TEST_CASE(MirrorsTheUnsignedHalf)
{
        // The signed half is the exact mirror of the unsigned half.
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<std::uint8_t>, std::int8_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<unsigned>, int>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<unsigned const>, int const>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<unsigned volatile>, int volatile>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<unsigned const volatile>, int const volatile>));
        XSTD_CONSTEXPR_CHECK(not has_make_signed<bool>);
        XSTD_CONSTEXPR_CHECK(not has_make_signed<double>);
        // An enumeration included, the mandated domain being std's to describe and not ours.
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<color>, std::make_signed_t<color>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<color const>, std::make_signed_t<color> const>));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ExactWidthIntegers, T, test::exact_width_unsigned_integer_types)
{
        static_assert(xstd::signed_integer<xstd::make_signed_t<T>>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
