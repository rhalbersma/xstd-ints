//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/bitwise_operators.hpp> // bitwise_operators
#include <xstd/ints/concepts/integer_class.hpp>     // integer_class
#include <test/exact_width_types.hpp>               // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>                 // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <bit>                                      // bit_width, countr_zero, popcount, rotl
#include <bitset>                                   // bitset
#include <concepts>                                 // integral

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(BitwiseOperators)

BOOST_AUTO_TEST_CASE_TEMPLATE(AdmitsTheUnsignedExactWidthTypesThroughCv, T, test::exact_width_unsigned_integer_types)
{
        static_assert(xstd::bitwise_operators<T>);
        static_assert(xstd::bitwise_operators<T const>);
        static_assert(xstd::bitwise_operators<T volatile>);
        static_assert(xstd::bitwise_operators<T const volatile>);
        BOOST_CHECK(true);
}

// The half that pins the signed_integer disjunct: std::integral does not see an integer class like absl::int128.
BOOST_AUTO_TEST_CASE_TEMPLATE(RejectsTheSignedExactWidthTypesThroughCv, T, test::exact_width_signed_integer_types)
{
        static_assert(not xstd::bitwise_operators<T>);
        static_assert(not xstd::bitwise_operators<T const>);
        static_assert(not xstd::bitwise_operators<T volatile>);
        static_assert(not xstd::bitwise_operators<T const volatile>);
        BOOST_CHECK(true);
}

// An integer must be an unsigned one. The standard unsigned integer types, and no other built-in.
BOOST_AUTO_TEST_CASE(TheStandardUnsignedIntegerTypes)
{
        static_assert(xstd::bitwise_operators<unsigned char>);
        static_assert(xstd::bitwise_operators<unsigned short>);
        static_assert(xstd::bitwise_operators<unsigned int>);
        static_assert(xstd::bitwise_operators<unsigned long>);
        static_assert(xstd::bitwise_operators<unsigned long long>);
        BOOST_CHECK(true);
}

// Every signed built-in goes, as the character types do: >> is arithmetic on them, preserving the sign.
BOOST_AUTO_TEST_CASE(TheSignedBuiltInsAreRefused)
{
        static_assert(not xstd::bitwise_operators<signed char>);
        static_assert(not xstd::bitwise_operators<short>);
        static_assert(not xstd::bitwise_operators<int>);
        static_assert(not xstd::bitwise_operators<long>);
        static_assert(not xstd::bitwise_operators<long long>);
        BOOST_CHECK(true);
}

// bool and the five character types are kept out by std::integral, not by any operator clause.
BOOST_AUTO_TEST_CASE(BoolAndTheCharacterTypesAreRefused)
{
        static_assert(not xstd::bitwise_operators<bool>);
        static_assert(not xstd::bitwise_operators<char>);
        static_assert(not xstd::bitwise_operators<wchar_t>);
        static_assert(not xstd::bitwise_operators<char8_t>);
        static_assert(not xstd::bitwise_operators<char16_t>);
        static_assert(not xstd::bitwise_operators<char32_t>);

        static_assert(not xstd::bitwise_operators<bool const>);
        static_assert(not xstd::bitwise_operators<char volatile>);
        static_assert(not xstd::bitwise_operators<char32_t const volatile>);

        // Still integral, and integer_class still admits the character types, so the narrowing is this concept's own.
        static_assert(std::integral<bool>);
        static_assert(xstd::integer_class<char8_t>);
        BOOST_CHECK(true);
}

// The boundary <bit> draws: over the built-ins this admits exactly what std::popcount accepts.
namespace {

template<class T>
concept has_bit_basis = requires (T x) { std::popcount(x); std::countr_zero(x); std::rotl(x, 1); std::bit_width(x); };

} // namespace

BOOST_AUTO_TEST_CASE(OverTheBuiltInsThisIsExactlyTheDomainOfBit)
{
        static_assert(xstd::bitwise_operators<unsigned char> == has_bit_basis<unsigned char>);
        static_assert(xstd::bitwise_operators<unsigned long long> == has_bit_basis<unsigned long long>);
        static_assert(xstd::bitwise_operators<bool> == has_bit_basis<bool>);
        static_assert(xstd::bitwise_operators<char> == has_bit_basis<char>);
        static_assert(xstd::bitwise_operators<char8_t> == has_bit_basis<char8_t>);
        static_assert(xstd::bitwise_operators<signed char> == has_bit_basis<signed char>);
        static_assert(xstd::bitwise_operators<int> == has_bit_basis<int>);
        static_assert(xstd::bitwise_operators<long long> == has_bit_basis<long long>);
        BOOST_CHECK(true);
}

// Where the two part company: a non-integer is judged by its operators alone, the disjunct admitting std::bitset.
BOOST_AUTO_TEST_CASE(AFieldOfBitsThatIsNotANumberIsAdmitted)
{
        static_assert(xstd::bitwise_operators<std::bitset<8>>);
        static_assert(xstd::bitwise_operators<std::bitset<64>>);
        static_assert(not has_bit_basis<std::bitset<64>>);
        static_assert(not std::integral<std::bitset<64>>);
        BOOST_CHECK(true);
}

namespace {

struct plain
{
};
enum unscoped_enum { unscoped_value };
enum class scoped_enum { scoped_value };

} // namespace

// What the operator clauses keep out: an enumeration promotes only so far, a float and a pointer have no bitwise op.
BOOST_AUTO_TEST_CASE(RejectsWhatIsNotAFieldOfBits)
{
        static_assert(not xstd::bitwise_operators<float>);
        static_assert(not xstd::bitwise_operators<double>);
        static_assert(not xstd::bitwise_operators<long double>);
        static_assert(not xstd::bitwise_operators<plain>);
        static_assert(not xstd::bitwise_operators<unscoped_enum>);
        static_assert(not xstd::bitwise_operators<scoped_enum>);
        static_assert(not xstd::bitwise_operators<int*>);
        static_assert(not xstd::bitwise_operators<void>);
        BOOST_CHECK(true);
}

// No ordering: integer_class asks a strong ordering under /9, and std::bitset has no operator< at all.
namespace {

template<class T>
concept ordered = requires (T const a, T const b) { a < b; };

} // namespace

BOOST_AUTO_TEST_CASE(NoOrderingIsRequired)
{
        static_assert(xstd::bitwise_operators<unsigned> and ordered<unsigned>);
        static_assert(xstd::bitwise_operators<std::bitset<8>> and not ordered<std::bitset<8>>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
