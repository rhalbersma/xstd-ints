//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/int128.hpp>            // XSTD_HAS_INT128, int128, uint128
#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/type_traits/is_signed.hpp>     // is_signed_v
#include <xstd/ints/type_traits/is_unsigned.hpp>   // is_unsigned_v
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <test/constexpr_check.hpp>                // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                                // same_as
#include <limits>                                  // numeric_limits

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdInt)
BOOST_AUTO_TEST_SUITE(Int128)

#ifdef XSTD_HAS_INT128

BOOST_AUTO_TEST_CASE(AssociatesSignedAndUnsignedPair)
{
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128>, xstd::int128>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128>, xstd::uint128>);
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128 const>, xstd::int128 const>);
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128 volatile>, xstd::int128 volatile>);
        static_assert(std::same_as<xstd::make_signed_t<xstd::uint128 const volatile>, xstd::int128 const volatile>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128 const>, xstd::uint128 const>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128 volatile>, xstd::uint128 volatile>);
        static_assert(std::same_as<xstd::make_unsigned_t<xstd::int128 const volatile>, xstd::uint128 const volatile>);
}

// The width the name promises, and the digit counts that follow from where the sign bit goes.
BOOST_AUTO_TEST_CASE(WidthAndDigits)
{
        static_assert(sizeof(xstd::int128) == 16);
        static_assert(sizeof(xstd::uint128) == 16);
        static_assert(std::numeric_limits<xstd::int128>::digits == 127);
        static_assert(std::numeric_limits<xstd::uint128>::digits == 128);
}

// Signedness as the standard trait, as this library's cv-transparent one, and as the concepts.
BOOST_AUTO_TEST_CASE(Signedness)
{
        static_assert(std::numeric_limits<xstd::int128>::is_signed);
        static_assert(not std::numeric_limits<xstd::uint128>::is_signed);
        static_assert(xstd::is_signed_v<xstd::int128 const>);
        static_assert(xstd::is_signed_v<xstd::int128 volatile>);
        static_assert(xstd::is_signed_v<xstd::int128 const volatile>);
        static_assert(xstd::is_unsigned_v<xstd::uint128 const>);
        static_assert(xstd::is_unsigned_v<xstd::uint128 volatile>);
        static_assert(xstd::is_unsigned_v<xstd::uint128 const volatile>);
        static_assert(xstd::signed_integer<xstd::int128>);
        static_assert(xstd::unsigned_integer<xstd::uint128>);
}

// Which the two preceding cases only assert about: that the extra width carries values.
BOOST_AUTO_TEST_CASE(Ordering)
{
        XSTD_CONSTEXPR_CHECK(xstd::int128{-1} < xstd::int128{0});
        XSTD_CONSTEXPR_CHECK((xstd::uint128{1} << 127U) > xstd::uint128{0});
}

#else

// Absent only where the target has no 128-bit integer of its own to alias, as for GCC and Clang on 32-bit x86 and ARM.
BOOST_AUTO_TEST_CASE(AbsentOnlyWhereTheTargetHasNone)
{
#if defined(__SIZEOF_INT128__) or defined(_MSC_VER)
        static_assert(false, "the target has a 128-bit integer, so XSTD_HAS_INT128 should be defined");
#endif
        BOOST_CHECK(true);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
