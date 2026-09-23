//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/bit_int.hpp>         // XSTD_HAS_BIT_INT, bit_int, bit_uint
#include <xstd/ints/cstdint/int128.hpp>          // int128, uint128
#include <xstd/ints/detail/delegates_to_std.hpp> // delegates_to_std
#include <boost/test/unit_test.hpp>              // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <concepts>                              // integral
#include <cstdint>                               // int8_t, int64_t, uint8_t, uint64_t

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Detail)
BOOST_AUTO_TEST_SUITE(DelegatesToStd)

BOOST_AUTO_TEST_CASE(StandardIntegers)
{
        static_assert(xstd::ints::detail::delegates_to_std<std::int8_t>);
        static_assert(xstd::ints::detail::delegates_to_std<std::uint8_t>);
        static_assert(xstd::ints::detail::delegates_to_std<std::int64_t>);
        static_assert(xstd::ints::detail::delegates_to_std<std::uint64_t>);
        BOOST_CHECK(true);
}

// The 128-bit pair delegates only where it is a builtin; a class is no std::integral.
BOOST_AUTO_TEST_CASE(OneTwentyEightBits)
{
        static_assert(xstd::ints::detail::delegates_to_std<xstd::int128> == std::integral<xstd::int128>);
        static_assert(xstd::ints::detail::delegates_to_std<xstd::uint128> == std::integral<xstd::uint128>);
        BOOST_CHECK(true);
}

#ifdef XSTD_HAS_BIT_INT
// Past the widest builtin, std::to_chars has no overload, however integral the type claims to be.
BOOST_AUTO_TEST_CASE(WiderThanStd)
{
        static_assert(not xstd::ints::detail::delegates_to_std<xstd::bit_int<256>>);
        static_assert(not xstd::ints::detail::delegates_to_std<xstd::bit_uint<256>>);
        BOOST_CHECK(true);
}
#endif // XSTD_HAS_BIT_INT

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
