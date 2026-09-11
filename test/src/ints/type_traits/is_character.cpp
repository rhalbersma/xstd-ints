//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/type_traits/is_character.hpp> // is_character, is_character_v
#include <test/constexpr_check.hpp>               // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>             // exact_width_integer_types
#include <boost/test/unit_test.hpp>               // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <type_traits>                            // bool_constant, is_same_v

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(TypeTraits)
BOOST_AUTO_TEST_SUITE(IsCharacter)

// [basic.fundamental]'s five, and no sixth. The list is the core language's, so this case is the whole trait.
BOOST_AUTO_TEST_CASE(TheFiveCharacterTypes)
{
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<char>);
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<wchar_t>);
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<char8_t>);
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<char16_t>);
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<char32_t>);
}

// The narrow character types that are NOT character types. signed char and unsigned char are standard integer
// types, which is what keeps int8_t and uint8_t -- a block type the bits library instantiates -- on the other
// side of every boundary drawn through this trait.
BOOST_AUTO_TEST_CASE(TheNarrowIntegerTypesAreNotCharacters)
{
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<signed char>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<unsigned char>);
}

// cv-transparent, as std's own traits are: the trait answers for the type, not for the spelling.
BOOST_AUTO_TEST_CASE(CvQualifiedSpellingsAgreeWithTheBareType)
{
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<char const>);
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<char volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<char const volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::is_character_v<char32_t const volatile>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<unsigned char const>);
}

// Total, as a trait must be: every other type answers false rather than hard-erroring, an incomplete class
// type included.
BOOST_AUTO_TEST_CASE(EverythingElseAnswersFalse)
{
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<bool>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<double>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<void>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<char*>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<char&>);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character_v<struct never_defined>);
}

// No exact-width alias is a character type, whatever the target spells them as.
BOOST_AUTO_TEST_CASE_TEMPLATE(NoExactWidthIntegerIsACharacter, T, test::exact_width_integer_types)
{
        static_assert(not xstd::is_character_v<T>);
        static_assert(not xstd::is_character_v<T const>);
        BOOST_CHECK(true);
}

// The class form tracks the variable, as std's trait pairs do.
BOOST_AUTO_TEST_CASE(TheClassFormTracksTheVariable)
{
        static_assert(std::is_same_v<xstd::is_character<char>, std::bool_constant<true>>);
        static_assert(std::is_same_v<xstd::is_character<int>, std::bool_constant<false>>);
        XSTD_CONSTEXPR_CHECK(xstd::is_character<char8_t>::value);
        XSTD_CONSTEXPR_CHECK(not xstd::is_character<unsigned char>::value);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
