//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/bit_mask.hpp>      // bit_mask
#include <xstd/ints/concepts/integer_class.hpp> // integer_class
#include <test/exact_width_types.hpp>           // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <bit>                                  // bit_width, countr_zero, popcount, rotl
#include <bitset>                               // bitset
#include <charconv>                             // chars_format
#include <concepts>                             // integral, signed_integral
#include <cstddef>                              // byte
#include <filesystem>                           // copy_options, directory_options, file_type, perm_options, perms
#include <future>                               // launch
#include <ios>                                  // ios_base
#include <regex>                                // regex_constants

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(BitMask)

BOOST_AUTO_TEST_CASE_TEMPLATE(AdmitsTheUnsignedExactWidthTypesThroughCv, T, test::exact_width_unsigned_integer_types)
{
        static_assert(xstd::bit_mask<T>);
        static_assert(xstd::bit_mask<T const>);
        static_assert(xstd::bit_mask<T volatile>);
        static_assert(xstd::bit_mask<T const volatile>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(RejectsTheSignedExactWidthTypesThroughCv, T, test::exact_width_signed_integer_types)
{
        static_assert(not xstd::bit_mask<T>);
        static_assert(not xstd::bit_mask<T const>);
        BOOST_CHECK(true);
}

// [bitmask.types] allows any integer type; this admits the unsigned ones, the narrow ones through promotion.
BOOST_AUTO_TEST_CASE(TheIntegerMembersAreTheUnsignedIntegers)
{
        static_assert(xstd::bit_mask<unsigned char>);
        static_assert(xstd::bit_mask<unsigned short>);
        static_assert(xstd::bit_mask<unsigned int>);
        static_assert(xstd::bit_mask<unsigned long long>);

        static_assert(not xstd::bit_mask<signed char>);
        static_assert(not xstd::bit_mask<int>);
        static_assert(not xstd::bit_mask<long long>);
        static_assert(not xstd::bit_mask<bool>);
        static_assert(not xstd::bit_mask<char>);
        static_assert(not xstd::bit_mask<char32_t>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(StdBitsetIsOne)
{
        static_assert(xstd::bit_mask<std::bitset<8>>);
        static_assert(xstd::bit_mask<std::bitset<100> const>);
        BOOST_CHECK(true);
}

// The standard's own bitmask types, each an enumeration overloading the operators in every library tested.
BOOST_AUTO_TEST_CASE(TheStandardBitmaskTypesAreAdmitted)
{
        static_assert(xstd::bit_mask<std::filesystem::perms>);
        static_assert(xstd::bit_mask<std::filesystem::perm_options>);
        static_assert(xstd::bit_mask<std::filesystem::copy_options>);
        static_assert(xstd::bit_mask<std::filesystem::directory_options>);
        static_assert(xstd::bit_mask<std::launch>);
        static_assert(xstd::bit_mask<std::chars_format>);
        static_assert(xstd::bit_mask<std::regex_constants::syntax_option_type>);
        static_assert(xstd::bit_mask<std::regex_constants::match_flag_type>);
        BOOST_CHECK(true);
}

// /4 asks only that the compound forms be valid: libstdc++'s ios_base flags return const X&, and are one too.
BOOST_AUTO_TEST_CASE(TheCompoundFormsNeedOnlyBeValid)
{
        // An implementation may make these a signed int, which the unsigned narrowing then turns away.
        static_assert(xstd::bit_mask<std::ios_base::fmtflags> or std::signed_integral<std::ios_base::fmtflags>);
        static_assert(xstd::bit_mask<std::ios_base::iostate> or std::signed_integral<std::ios_base::iostate>);
        static_assert(xstd::bit_mask<std::ios_base::openmode> or std::signed_integral<std::ios_base::openmode>);
        BOOST_CHECK(true);
}

// An enumeration is one by its operators alone, whatever the standard calls it.
BOOST_AUTO_TEST_CASE(AnEnumerationIsOneByItsOperators)
{
        // [cstddef.syn] never calls it a bitmask type, yet it overloads every operator one needs.
        static_assert(xstd::bit_mask<std::byte>);

        // [enumerated.types], declared beside perms and syntax_option_type, with no operators of its own.
        static_assert(not xstd::bit_mask<std::filesystem::file_type>);
        static_assert(not xstd::bit_mask<std::regex_constants::error_type>);
        BOOST_CHECK(true);
}

// bool and the five character types are kept out by std::integral, not by any operator clause.
BOOST_AUTO_TEST_CASE(BoolAndTheCharacterTypesAreRefusedThroughCv)
{
        static_assert(not xstd::bit_mask<bool const>);
        static_assert(not xstd::bit_mask<wchar_t>);
        static_assert(not xstd::bit_mask<char8_t>);
        static_assert(not xstd::bit_mask<char16_t volatile>);
        static_assert(not xstd::bit_mask<char32_t const volatile>);

        // Still integral, and integer_class still admits the character types, so the narrowing is this concept's own.
        static_assert(std::integral<bool>);
        static_assert(xstd::integer_class<char8_t>);
        BOOST_CHECK(true);
}

namespace {

template<class T>
concept has_bit_basis = requires (T x) { std::popcount(x); std::countr_zero(x); std::rotl(x, 1); std::bit_width(x); };

template<class T>
concept ordered = requires (T const a, T const b) { a < b; };

} // namespace

// The boundary <bit> draws: over the built-ins this admits exactly what std::popcount accepts.
BOOST_AUTO_TEST_CASE(OverTheBuiltInsThisIsExactlyTheDomainOfBit)
{
        static_assert(xstd::bit_mask<unsigned char> == has_bit_basis<unsigned char>);
        static_assert(xstd::bit_mask<unsigned long long> == has_bit_basis<unsigned long long>);
        static_assert(xstd::bit_mask<bool> == has_bit_basis<bool>);
        static_assert(xstd::bit_mask<char> == has_bit_basis<char>);
        static_assert(xstd::bit_mask<signed char> == has_bit_basis<signed char>);
        static_assert(xstd::bit_mask<int> == has_bit_basis<int>);
        static_assert(not has_bit_basis<std::bitset<64>> and not std::integral<std::bitset<64>>);
        BOOST_CHECK(true);
}

// No ordering is asked for: std::bitset has no operator< at all.
BOOST_AUTO_TEST_CASE(NoOrderingIsRequired)
{
        static_assert(xstd::bit_mask<unsigned> and ordered<unsigned>);
        static_assert(xstd::bit_mask<std::bitset<8>> and not ordered<std::bitset<8>>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
