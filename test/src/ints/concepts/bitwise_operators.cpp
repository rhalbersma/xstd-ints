//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/bitwise_operators.hpp> // bitwise_operators
#include <xstd/ints/concepts/integer_class.hpp>     // integer_class
#include <xstd/ints/type_traits/is_character.hpp>   // is_character_v
#include <test/exact_width_types.hpp>               // exact_width_integer_types
#include <boost/test/unit_test.hpp>                 // Boost.Test
#include <concepts>                                 // integral
#include <cstdint>                                  // int8_t, uint8_t

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(BitwiseOperators)

BOOST_AUTO_TEST_CASE_TEMPLATE(AdmitsTheExactWidthTypesThroughCv, T, test::exact_width_integer_types)
{
        static_assert(xstd::bitwise_operators<T>);
        static_assert(xstd::bitwise_operators<T const>);
        static_assert(xstd::bitwise_operators<T volatile>);
        static_assert(xstd::bitwise_operators<T const volatile>);
        BOOST_CHECK(true);
}

// Everything integer_class admits this admits too, but for the six excluded by name: pruning requirements
// widens the set, and the named boundary is what narrows it back.
BOOST_AUTO_TEST_CASE(StandardIntegral)
{
        static_assert(xstd::bitwise_operators<signed char>);
        static_assert(xstd::bitwise_operators<unsigned char>);
        static_assert(xstd::bitwise_operators<short>);
        static_assert(xstd::bitwise_operators<unsigned short>);
        static_assert(xstd::bitwise_operators<int>);
        static_assert(xstd::bitwise_operators<unsigned int>);
        static_assert(xstd::bitwise_operators<long>);
        static_assert(xstd::bitwise_operators<unsigned long>);
        static_assert(xstd::bitwise_operators<long long>);
        static_assert(xstd::bitwise_operators<unsigned long long>);
        BOOST_CHECK(true);
}

// bool answers every requirement -- ~b, b &= b and b <<= n all compile through integral promotion -- so the
// pruning would let it in and only the named exclusion keeps it out. What disqualifies it is semantic: the
// conversion back from the promoted int is a nonzero test rather than a reduction, so the two operators that
// need wraparound give the wrong answer while the three that do not come out right by accident.
//
// This case deliberately does NOT demonstrate that by writing the expressions out, because neither compiler
// will let it: ~b is -Wbool-operation on both, b <<= 1 is -Wint-in-bool-context on GCC, and GCC diagnoses
// them even inside an unevaluated requires-expression. That is the corroboration rather than a nuisance --
// both front ends already call these a mistake, and the concept is now saying the same thing in its own
// vocabulary. The exclusion is pinned here by its result; the reasoning lives in the header.
BOOST_AUTO_TEST_CASE(BoolIsExcludedByName)
{
        static_assert(not xstd::bitwise_operators<bool>);
        static_assert(not xstd::bitwise_operators<bool const>);
        static_assert(not xstd::bitwise_operators<bool volatile>);
        static_assert(not xstd::bitwise_operators<bool const volatile>);

        // Still an integral type with every operator available, so nothing structural is what rejects it.
        static_assert(std::integral<bool>);

        BOOST_CHECK(true);
}

// The five character types go the same way. char and wchar_t have implementation-defined signedness, so >>
// may sign-extend and the same source yields different bits per target; char8_t, char16_t and char32_t are
// unsigned and modular and would be sound, and go because they are text and every width they reach is
// reached better by the uintN_t that names it.
BOOST_AUTO_TEST_CASE(TheCharacterTypesAreExcludedByNameThroughCv)
{
        static_assert(not xstd::bitwise_operators<char>);
        static_assert(not xstd::bitwise_operators<wchar_t>);
        static_assert(not xstd::bitwise_operators<char8_t>);
        static_assert(not xstd::bitwise_operators<char16_t>);
        static_assert(not xstd::bitwise_operators<char32_t>);

        static_assert(not xstd::bitwise_operators<char const>);
        static_assert(not xstd::bitwise_operators<char8_t volatile>);
        static_assert(not xstd::bitwise_operators<char32_t const volatile>);

        // integer_class still admits them, so this narrowing is the concept's own and not inherited.
        static_assert(xstd::integer_class<char8_t>);

        BOOST_CHECK(true);
}

// What the exclusion must NOT reach. signed char and unsigned char are narrow character types but standard
// integer types, so int8_t and uint8_t keep their place -- and uint8_t is a block type the bits library
// actually instantiates.
BOOST_AUTO_TEST_CASE(TheNarrowIntegerTypesSurviveTheExclusion)
{
        static_assert(not xstd::is_character_v<signed char>);
        static_assert(not xstd::is_character_v<unsigned char>);
        static_assert(xstd::bitwise_operators<std::int8_t>);
        static_assert(xstd::bitwise_operators<std::uint8_t>);
        BOOST_CHECK(true);
}

namespace {

struct plain
{
};
enum unscoped_enum { unscoped_value };
enum class scoped_enum { scoped_value };

} // namespace

// What stays out. An enumeration promotes far enough for ~e and e & e, and no further: there is no assignment
// back into the enumeration, so the compound forms reject it. A floating type has no bitwise operator at all,
// and a pointer's arithmetic is not bitwise.
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
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
