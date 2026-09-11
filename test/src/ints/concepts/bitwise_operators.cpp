//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/bitwise_operators.hpp> // bitwise_operators
#include <xstd/ints/concepts/integer_class.hpp>     // integer_class
#include <test/exact_width_types.hpp>               // exact_width_integer_types
#include <boost/test/unit_test.hpp>                 // Boost.Test

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

// Everything integer_class admits, this admits: pruning requirements can only widen the set.
BOOST_AUTO_TEST_CASE(StandardIntegral)
{
        static_assert(xstd::bitwise_operators<char>);
        static_assert(xstd::bitwise_operators<signed char>);
        static_assert(xstd::bitwise_operators<unsigned char>);
        static_assert(xstd::bitwise_operators<char8_t>);
        static_assert(xstd::bitwise_operators<char16_t>);
        static_assert(xstd::bitwise_operators<char32_t>);
        static_assert(xstd::bitwise_operators<wchar_t>);
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

// The one type the pruning lets in that integer_class keeps out. bool has every operator asked for, through
// integral promotion -- ~b, b &= b and b <<= n all compile -- and integer_class excludes it only because C++17
// removed its ++ and --, which are not bitwise and are not asked for here. Pinned rather than left implicit:
// the admission is a consequence of what was pruned, so it should fail loudly if the pruning changes.
BOOST_AUTO_TEST_CASE(BoolIsAdmittedWhereIntegerClassExcludesIt)
{
        static_assert(xstd::bitwise_operators<bool>);
        static_assert(not xstd::integer_class<bool>);
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
