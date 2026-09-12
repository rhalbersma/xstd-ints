//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/countr_zero.hpp> // countr_zero
#include <xstd/ints/bit/popcount.hpp>    // popcount
#include <xstd/ints/cstdint.hpp>         // uint128
#include <xstd/ints/limits.hpp>          // numeric_limits
#include <test/constexpr_check.hpp>      // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>    // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <concepts>                      // unsigned_integral
#include <bit>                           // countr_zero
#include <cstdint>                       // uint64_t
#include <tuple>                         // tuple_cat
#include <utility>                       // declval

namespace {

// Every exact width but the bit-precise: unsigned _BitInt(N) reaches neither <bit> nor any overload here, carrying no words to read. xstd::uint128 is here because the tests build as gnu++.
template<class T>
concept has_popcount = requires (T x) { xstd::popcount(x); };

using worded_unsigned_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>()));

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(CountrZero)

// Where <bit> answers, this IS <bit>: the overload forwards and must not drift from it.
BOOST_AUTO_TEST_CASE(AgreesWithStdWhereStdAnswers)
{
        XSTD_CONSTEXPR_CHECK(xstd::countr_zero(std::uint64_t{0}) == std::countr_zero(std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::countr_zero(std::uint64_t{1}) == std::countr_zero(std::uint64_t{1}));
        XSTD_CONSTEXPR_CHECK(xstd::countr_zero(~std::uint64_t{0}) == std::countr_zero(~std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::countr_zero(std::uint64_t{1} << 63U) == std::countr_zero(std::uint64_t{1} << 63U));
}

// Total across every width the library carries, the 128-bit classes included, which is where this reaches past <bit>.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsOfEveryWidth, T, worded_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;
        static_assert(xstd::countr_zero(T{0}) == W);
        static_assert(xstd::countr_zero(T{1}) == 0);
        static_assert(xstd::countr_zero(xstd::numeric_limits<T>::max()) == 0);
        BOOST_CHECK(true);
}

// The word boundary, which pins each class's low half to its low half: read through the wrong accessor a type answers with the halves swapped.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheWordBoundary, T, worded_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;
        if constexpr (W >= 128) {
                constexpr auto bit64 = static_cast<T>(T{1} << 64U);
                static_assert(xstd::countr_zero(bit64) == 64);
        }
        BOOST_CHECK(true);
}

// The constraint is exactly what the body needs, asserted as an equality so it fails as loudly if it ever became wider OR narrower than <bit>.
namespace {

template<class T>
concept xstd_answers = requires (T x) { xstd::countr_zero(x); };

template<class T>
concept std_answers = requires (T x) { std::countr_zero(x); };

} // namespace

BOOST_AUTO_TEST_CASE(TheConstraintIsTheBodys)
{
        static_assert(xstd_answers<bool> == std_answers<bool>);
        static_assert(xstd_answers<char8_t> == std_answers<char8_t>);
        static_assert(xstd_answers<char16_t> == std_answers<char16_t>);
        static_assert(xstd_answers<char32_t> == std_answers<char32_t>);
        static_assert(xstd_answers<unsigned char> == std_answers<unsigned char>);
        static_assert(xstd_answers<std::uint64_t> == std_answers<std::uint64_t>);

        // NOT an equality at the widest exact width: where xstd::uint128 is a class <bit> declines it and the overload beside its own header answers, so this asserts a basis exists rather than that std has one.
        static_assert(xstd_answers<xstd::uint128>);

#ifdef XSTD_HAS_BIT_INT
        // The tripwire for P3666R4: when is_integral_v<_BitInt(N)> goes true while <bit> still refuses it, this equality fails by name instead of the header hard-erroring at whatever first calls it.
        static_assert(xstd_answers<xstd::bit_uint<64>> == std_answers<xstd::bit_uint<64>>);
        static_assert(xstd_answers<xstd::bit_uint<24>> == std_answers<xstd::bit_uint<24>>);
#endif

        // And those first four really are the gap std::unsigned_integral would have opened.
        static_assert(std::unsigned_integral<bool> and not std_answers<bool>);
        static_assert(std::unsigned_integral<char8_t> and not std_answers<char8_t>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
