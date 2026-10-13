//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts.hpp>   // the integer concepts
#include <xstd/ints/cstdint.hpp>    // XSTD_HAS_INT128, int128, uint128
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <complex>                  // complex
#include <cstddef>                  // size_t
#include <cstdint>                  // exact-width integer types

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)

// How the six stand to one another, asked of every type below: each clause is a refinement no single header can see.
template<class T>
concept lattice =
        // integer is the two halves, and nothing is both or neither.
        ((xstd::signed_integer<T> or xstd::unsigned_integer<T>) == xstd::integer<T>) and
        not(xstd::signed_integer<T> and xstd::unsigned_integer<T>) and
        // integer_class is integer's opening clause, so integer is the narrower of the two.
        (not xstd::integer<T> or xstd::integer_class<T>) and
        // nothrow_const_operators opens on integer too, and adds only the specification.
        (not xstd::nothrow_const_operators<T> or xstd::integer<T>) and
        // alignable admits the unsigned half and refuses the signed one.
        (not xstd::unsigned_integer<T> or xstd::alignable<T>) and
        (not xstd::signed_integer<T> or not xstd::alignable<T>) and
        // and its nothrow refinement is a refinement.
        (not xstd::nothrow_alignable<T> or xstd::alignable<T>);

BOOST_AUTO_TEST_CASE(TheRefinementsHoldOverTheWholeDomain)
{
        static_assert(lattice<signed char>);
        static_assert(lattice<unsigned char>);
        static_assert(lattice<short>);
        static_assert(lattice<unsigned short>);
        static_assert(lattice<int>);
        static_assert(lattice<unsigned>);
        static_assert(lattice<long long>);
        static_assert(lattice<unsigned long long>);
        static_assert(lattice<std::size_t>);
        static_assert(lattice<std::int8_t>);
        static_assert(lattice<std::uint64_t>);
#ifdef XSTD_HAS_INT128
        static_assert(lattice<xstd::int128>);
        static_assert(lattice<xstd::uint128>);
#endif

        // The types each clause turns away, so no clause is vacuous over this list.
        static_assert(lattice<bool>);
        static_assert(lattice<char>);
        static_assert(lattice<char32_t>);
        static_assert(lattice<double>);
        static_assert(lattice<void>);
        static_assert(lattice<int*>);
        static_assert(lattice<int&>);
        static_assert(lattice<std::complex<double>>);
        BOOST_CHECK(true);
}

// All six strip cv, so the lattice is the same qualified, including the 128-bit type that is a class on one toolchain.
BOOST_AUTO_TEST_CASE(TheRefinementsSurviveCvQualification)
{
        static_assert(lattice<int const>);
        static_assert(lattice<int volatile>);
        static_assert(lattice<int const volatile>);
        static_assert(lattice<unsigned const>);
        static_assert(lattice<unsigned volatile>);
        static_assert(lattice<unsigned const volatile>);
        static_assert(lattice<std::size_t const volatile>);
#ifdef XSTD_HAS_INT128
        static_assert(lattice<xstd::uint128 const volatile>);
        static_assert(lattice<xstd::int128 const volatile>);
#endif

        static_assert(xstd::integer<int const> == xstd::integer<int>);
        static_assert(xstd::alignable<unsigned const volatile> == xstd::alignable<unsigned>);
#ifdef XSTD_HAS_INT128
        static_assert(xstd::alignable<xstd::uint128 const volatile> == xstd::alignable<xstd::uint128>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
