//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <test/constexpr_check.hpp>                // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>              // exact-width integer types
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                 // complex
#include <concepts>                                // same_as
#include <cstdint>                                 // exact-width integer types
#include <type_traits>                             // make_unsigned_t

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(MakeUnsigned)

enum class color : unsigned
{
        red = 1
};

// A named concept: a non-dependent invalid operand is a hard error on GCC.
template<class T>
concept has_make_unsigned = requires { typename xstd::make_unsigned_t<T>; };

BOOST_AUTO_TEST_CASE(MirrorsTheSignedHalf)
{
        // agrees with std::make_unsigned wherever it answers, unsigned as much as signed
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<std::int8_t>, std::uint8_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<std::int16_t>, std::uint16_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<std::int32_t>, std::uint32_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<std::int64_t>, std::uint64_t>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<int>, std::make_unsigned_t<int>>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<unsigned>, unsigned>));

        // including the enumerations /2 mandates, as the equality: the association is std's to make.
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<color>, std::make_unsigned_t<color>>));

        // and answers no where std::make_unsigned hard-errors: the point of an empty primary
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<bool>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<double>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<int*>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<std::complex<double>>);
        XSTD_CONSTEXPR_CHECK(not has_make_unsigned<void>);

        // cv-qualification is preserved, just as by std::make_unsigned.
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<int const>, unsigned const>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<int volatile>, unsigned volatile>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<int const volatile>, unsigned const volatile>));
        XSTD_CONSTEXPR_CHECK(std::is_const_v<xstd::make_unsigned_t<color const>>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ExactWidthIntegers, T, test::exact_width_signed_integer_types)
{
        static_assert(xstd::unsigned_integer<xstd::make_unsigned_t<T>>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
