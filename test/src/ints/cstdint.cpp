//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint.hpp>                   // XSTD_HAS_BIT_INT, XSTD_HAS_INT128, bit_int, bit_uint, int128, uint128
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                                // same_as

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdInt)

// What the two headers have in common: each names a pair, not a type, which neither can state alone.
template<class S, class U>
concept pair =
        std::same_as<xstd::make_unsigned_t<S>, U> and
        std::same_as<xstd::make_signed_t<U>, S> and
        // The round trip is the identity from either end.
        std::same_as<xstd::make_signed_t<xstd::make_unsigned_t<S>>, S> and
        std::same_as<xstd::make_unsigned_t<xstd::make_signed_t<U>>, U> and
        // The unsigned half spends on magnitude the bit the signed half spends on sign.
        (xstd::numeric_limits<U>::digits == xstd::numeric_limits<S>::digits + 1) and
        xstd::numeric_limits<S>::is_signed and not xstd::numeric_limits<U>::is_signed;

BOOST_AUTO_TEST_CASE(EveryTypeNamedHereIsHalfOfAPair)
{
#ifdef XSTD_HAS_INT128
        static_assert(pair<xstd::int128, xstd::uint128>);
#endif
#ifdef XSTD_HAS_BIT_INT
        static_assert(pair<xstd::bit_int<2>, xstd::bit_uint<2>>);
        static_assert(pair<xstd::bit_int<17>, xstd::bit_uint<17>>);
        static_assert(pair<xstd::bit_int<64>, xstd::bit_uint<64>>);
#endif
        BOOST_CHECK(true);
}

// And the pairing is cv-transparent across both, the qualifier riding through.
BOOST_AUTO_TEST_CASE(ThePairingCarriesCvQualification)
{
#ifdef XSTD_HAS_INT128
        static_assert(pair<xstd::int128 const, xstd::uint128 const>);
        static_assert(pair<xstd::int128 volatile, xstd::uint128 volatile>);
#endif
#ifdef XSTD_HAS_BIT_INT
        static_assert(pair<xstd::bit_int<17> const, xstd::bit_uint<17> const>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
