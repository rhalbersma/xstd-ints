//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdint.hpp>                   // int128, uint128
#include <xstd/ints/limits.hpp>                    // numeric_limits
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <test/constexpr_check.hpp>                // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                                // same_as
#include <cstdint>                                 // uint64_t

// Reached the way a consumer reaches it: the probe here, the adapters behind the umbrella, which needs both libraries.
#if __has_include(<boost/int128.hpp>) and __has_include(<boost/hash2/hash_append.hpp>)
#define TEST_HAS_BOOST_INT128_AND_HASH2
#include <xstd/ints/ext/boost.hpp> // the adapters for boost::int128's pair and for Boost.Hash2
#include <boost/hash2/flavor.hpp>  // little_endian_flavor
#include <boost/hash2/fnv1a.hpp>   // fnv1a_64
#endif

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)

BOOST_AUTO_TEST_CASE(TheAssociationsArriveThroughTheUmbrella)
{
#ifdef TEST_HAS_BOOST_INT128_AND_HASH2
        using S = boost::int128::int128;
        using U = boost::int128::uint128;

        static_assert(std::same_as<xstd::make_unsigned_t<S>, U>);
        static_assert(std::same_as<xstd::make_signed_t<U>, S>);
        static_assert(xstd::signed_integer<S>);
        static_assert(xstd::unsigned_integer<U>);
#endif
        BOOST_CHECK(true);
}

namespace {

#ifdef TEST_HAS_BOOST_INT128_AND_HASH2
template<class T>
[[nodiscard]] constexpr auto digest_of(T v)
        -> std::uint64_t
{
        auto h = boost::hash2::fnv1a_64{};
        xstd::hash_append_int(h, boost::hash2::little_endian_flavor{}, v);
        return h.result();
}
#endif

} // namespace

// What spans the two: the pair makes Boost's types integers, and so hashable, as the library's own of their width.
BOOST_AUTO_TEST_CASE(TheirPairHashesAsTheLibrarysOwn)
{
#ifdef TEST_HAS_BOOST_INT128_AND_HASH2
        using S = boost::int128::int128;
        using U = boost::int128::uint128;

        XSTD_CONSTEXPR_CHECK(digest_of(S{-1}) == digest_of(xstd::int128{-1}));
        XSTD_CONSTEXPR_CHECK(digest_of(U{1} << 100U) == digest_of(xstd::uint128{1} << 100U));
        XSTD_CONSTEXPR_CHECK(digest_of(xstd::numeric_limits<S>::min()) == digest_of(xstd::numeric_limits<xstd::int128>::min()));
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
