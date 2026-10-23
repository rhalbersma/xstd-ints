//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/popcount.hpp>              // popcount
#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdlib/div.hpp>               // div
#include <xstd/ints/limits.hpp>                    // numeric_limits
#include <xstd/ints/memory/align_up.hpp>           // align_up
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned
#include <test/constexpr_check.hpp>                // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK
#include <concepts>                                // same_as

// Reached the way a consumer reaches it: the probe here, the pair inside the adapter.
#if __has_include(<boost/int128.hpp>)
#define TEST_HAS_BOOST_INT128
#include <xstd/ints/ext/boost/int128.hpp> // int128, uint128
#endif

namespace {

template<class T>
concept divides_by_literal = requires (T x) { xstd::div(x, 2); };

template<class T>
concept counts_bits = requires (T x) { xstd::popcount(x); };

template<class T>
concept aligns = requires (T x) { xstd::align_up(x, 64); };

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(Int128)

BOOST_AUTO_TEST_CASE(AssociationsPairTheTwoHalves)
{
#ifdef TEST_HAS_BOOST_INT128
        using S = boost::int128::int128;
        using U = boost::int128::uint128;

        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<S>, U>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<U>, S>));

        // Each half is its own, which is how integer tells a pair from a third type beside one.
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<S>, S>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<U>, U>));

        // [iterator.concept.winc]/3's range, of the pair this header supplies.
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<S>::min() + xstd::numeric_limits<S>::max() == -1);
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<U>::min() == 0);
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<U>::digits == xstd::numeric_limits<S>::digits + 1);

        // Each half reached from the other and back, and the cv qualifiers carried along.
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<xstd::make_unsigned_t<S>>, S>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<xstd::make_signed_t<U>>, U>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_signed_t<U const>, S const>));
        XSTD_CONSTEXPR_CHECK((std::same_as<xstd::make_unsigned_t<S volatile>, U volatile>));
#endif
        BOOST_CHECK(true);
}

// Which is the whole of what integer asks beyond integer_class, and what the shipped header supplies.
BOOST_AUTO_TEST_CASE(WhichIsWhatCarriesThePairIntoTheConcepts)
{
#ifdef TEST_HAS_BOOST_INT128
        XSTD_CONSTEXPR_CHECK(xstd::signed_integer<boost::int128::int128>);
        XSTD_CONSTEXPR_CHECK(xstd::unsigned_integer<boost::int128::uint128>);
#endif
        BOOST_CHECK(true);
}

// The mistakes that port from built-in code are turned away at compile time rather than converted.
BOOST_AUTO_TEST_CASE(NearMissesStayRejected)
{
#ifdef TEST_HAS_BOOST_INT128
        using S = boost::int128::int128;

        // A literal divisor, as in x / 10, deduces int beside the class instead of widening.
        static_assert(not divides_by_literal<S>);

        // A signed class has no bits to count, though Boost's own popcount takes one through its conversion to uint128.
        static_assert(not counts_bits<S>);

        // Nor an address to align, which a signed type would let wrap into undefined behaviour.
        static_assert(not aligns<S>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
