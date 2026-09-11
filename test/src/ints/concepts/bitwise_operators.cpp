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

// The other half of the same list, and the half that pins the signed_integer disjunct: std::integral does not
// see an integer class, so without it absl::int128 and boost::int128 would be admitted here while long long
// was refused. This case is where those two types are reached, since neither exists on every target.
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

// Every signed built-in goes, and for the reason the character types go: >> is arithmetic on them, preserving
// the sign rather than moving the bits. char and wchar_t are only implementation-definedly signed; these are
// signed always, so if the one disqualifies the other does a fortiori.
BOOST_AUTO_TEST_CASE(TheSignedBuiltInsAreRefused)
{
        static_assert(not xstd::bitwise_operators<signed char>);
        static_assert(not xstd::bitwise_operators<short>);
        static_assert(not xstd::bitwise_operators<int>);
        static_assert(not xstd::bitwise_operators<long>);
        static_assert(not xstd::bitwise_operators<long long>);
        BOOST_CHECK(true);
}

// bool and the five character types. Each answers every operator requirement through integral promotion, so
// nothing structural rejects them -- std::integral is what does, by keeping them inside the net that
// unsigned_integer then refuses. bool's own failure is that its conversion back from the promoted int is a
// nonzero test rather than a reduction, which is also why this case cannot demonstrate it: ~b is
// -Wbool-operation on GCC and Clang alike and b <<= 1 is -Wint-in-bool-context on GCC, and GCC diagnoses both
// even inside an unevaluated requires-expression. Both front ends already call these a mistake.
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

        // Still integral, and integer_class still admits the character types, so the narrowing is this
        // concept's own rather than something it inherited.
        static_assert(std::integral<bool>);
        static_assert(xstd::integer_class<char8_t>);
        BOOST_CHECK(true);
}

// The boundary is the one <bit> already draws. Over the built-ins this concept admits exactly the types
// std::popcount and its neighbours accept, which is what makes it the concept a bit container's storage
// answers in.
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

// And where the two part company, deliberately: a type that is no kind of integer is judged by its operators
// alone, which is the disjunct that admits a field of bits that is not a number. std::bitset answers no <bit>
// function and belongs here anyway -- it is the type the whole concept is shaped after.
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

// What the operator clauses still keep out on their own, the gate having let them through. An enumeration
// promotes far enough for ~e and e & e, and no further: there is no assignment back into the enumeration, so
// the compound forms reject it. A floating type has no bitwise operator at all, and a pointer's arithmetic is
// not bitwise.
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

// The ordering is deliberately absent. integer_class asks for a strong ordering under /9; this does not,
// because std::bitset -- the type the concept is shaped after -- has no operator< at all. Asking for one would
// exclude it, so a bit container's order stays its wrapper's business.
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
