//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/bit_int.hpp>           // XSTD_HAS_BIT_INT, bit_int, bit_int_max_width, bit_uint
#include <xstd/ints/concepts/integer.hpp>          // integer
#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <test/constexpr_check.hpp>                // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <concepts>                                // same_as
#include <cstddef>                                 // size_t
#include <cstdint>                                 // int8_t, int64_t, uint8_t, uint64_t
#include <limits>                                  // numeric_limits

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdInt)
BOOST_AUTO_TEST_SUITE(BitInt)

#ifdef XSTD_HAS_BIT_INT

// A named concept, not a bare requires-expression: naming an alias whose constraint fails is ill-formed.
template<std::size_t N>
concept has_bit_int = requires {
        typename xstd::bit_int<N>;
        typename xstd::bit_uint<N>;
};

BOOST_AUTO_TEST_CASE(AliasesNameTheBitPreciseTypes)
{
        static_assert(std::same_as<xstd::bit_int<17>, signed _BitInt(17)>);
        static_assert(std::same_as<xstd::bit_uint<17>, unsigned _BitInt(17)>);
        static_assert(std::same_as<decltype(xstd::bit_int_max_width), std::size_t const>);
        static_assert(xstd::bit_int_max_width == __BITINT_MAXWIDTH__);
        static_assert(has_bit_int<2> and has_bit_int<xstd::bit_int_max_width>);
        static_assert(not has_bit_int<1> and not has_bit_int<xstd::bit_int_max_width + 1>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(AliasesModelTheIntegerConcepts)
{
        using S = xstd::bit_int<17>;
        using U = xstd::bit_uint<17>;

        static_assert(xstd::integer<S>);
        static_assert(xstd::integer<U>);
        static_assert(xstd::signed_integer<S>);
        static_assert(xstd::unsigned_integer<U>);
        static_assert(std::same_as<xstd::make_unsigned_t<S>, U>);
        static_assert(std::same_as<xstd::make_signed_t<U>, S>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(LimitsFollowTheRequestedWidth)
{
        using S = xstd::bit_int<17>;
        using U = xstd::bit_uint<17>;

        static_assert(xstd::numeric_limits<S>::digits == 16);
        static_assert(xstd::numeric_limits<U>::digits == 17);
        static_assert(xstd::numeric_limits<S>::is_signed and not xstd::numeric_limits<U>::is_signed);
        static_assert(xstd::numeric_limits<S>::is_exact and xstd::numeric_limits<U>::is_exact);
        static_assert(xstd::numeric_limits<S>::is_bounded and xstd::numeric_limits<U>::is_bounded);
        static_assert(not xstd::numeric_limits<S>::is_modulo and xstd::numeric_limits<U>::is_modulo);
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<S>::lowest() == xstd::numeric_limits<S>::min());
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<U>::lowest() == xstd::numeric_limits<U>::min());
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<S>::min() == S{-65536});
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<S>::max() == S{65535});
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<U>::max() == U{131071});

        // [iterator.concept.winc]/3's range, stated without naming the width.
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<S>::min() + xstd::numeric_limits<S>::max() == -1);
        XSTD_CONSTEXPR_CHECK(xstd::numeric_limits<U>::min() == 0);
        XSTD_CONSTEXPR_CHECK((static_cast<U>(xstd::numeric_limits<S>::max()) * 2) + 1 == xstd::numeric_limits<U>::max());
}

// libstdc++ leaves digits10 at 0 and libc++ takes it from sizeof, as a 32-bit integer for bit_uint<24>.
BOOST_AUTO_TEST_CASE(Digits10CountsTheDecimalDigitsEveryValueHas)
{
        static_assert(xstd::numeric_limits<xstd::bit_uint<24>>::digits10 == 7);
        static_assert(xstd::numeric_limits<xstd::bit_int<24>>::digits10 == 6);
        static_assert(xstd::numeric_limits<xstd::bit_uint<8>>::digits10 == std::numeric_limits<std::uint8_t>::digits10);
        static_assert(xstd::numeric_limits<xstd::bit_int<8>>::digits10 == std::numeric_limits<std::int8_t>::digits10);
        static_assert(xstd::numeric_limits<xstd::bit_uint<64>>::digits10 == std::numeric_limits<std::uint64_t>::digits10);
        static_assert(xstd::numeric_limits<xstd::bit_int<64>>::digits10 == std::numeric_limits<std::int64_t>::digits10);
        static_assert(xstd::numeric_limits<xstd::bit_uint<24>>::max_digits10 == 0);
        BOOST_CHECK(true);
}

template<class T>
concept has_signed_counterpart = requires { typename xstd::make_signed_t<T>; };

// A one-bit flag spelled unsigned _BitInt(1) has no signed half until Clang implements N3747, so it is no integer.
BOOST_AUTO_TEST_CASE(OneBitHasNoSignedCounterpart)
{
        static_assert(not has_signed_counterpart<unsigned _BitInt(1)>);
        static_assert(not has_signed_counterpart<unsigned _BitInt(1) const volatile>);
        static_assert(std::same_as<xstd::make_unsigned_t<unsigned _BitInt(1)>, unsigned _BitInt(1)>);
        static_assert(std::same_as<xstd::make_unsigned_t<unsigned _BitInt(1) const>, unsigned _BitInt(1) const>);
        static_assert(not xstd::integer<unsigned _BitInt(1)>);
        static_assert(not xstd::unsigned_integer<unsigned _BitInt(1) volatile>);
        BOOST_CHECK(true);
}

#else

// The aliases are not declared without __BITINT_MAXWIDTH__, and Boost.Test wants a case either way.
BOOST_AUTO_TEST_CASE(AbsentOnlyWithoutCompilerSupport)
{
#ifdef __BITINT_MAXWIDTH__
        static_assert(false, "the compiler has _BitInt, so XSTD_HAS_BIT_INT should be defined");
#endif
        BOOST_CHECK(true);
}

#endif // XSTD_HAS_BIT_INT

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
