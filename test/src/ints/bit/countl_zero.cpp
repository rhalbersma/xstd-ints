//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/countl_zero.hpp> // countl_zero
#include <xstd/ints/bit/popcount.hpp>    // popcount
#include <xstd/ints/cstdint.hpp>         // uint128
#include <xstd/ints/limits.hpp>          // numeric_limits
#include <test/constexpr_check.hpp>      // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>    // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <concepts>                      // unsigned_integral
#include <bit>                           // countl_zero
#include <cstdint>                       // uint64_t
#include <tuple>                         // tuple_cat
#include <utility>                       // declval

namespace {

// Every exact width except the bit-precise ones: unsigned _BitInt(N) reaches neither <bit> -- libstdc++
// constrains these functions to the five standard unsigned types by is_same -- nor any overload here, and
// unlike the 128-bit classes it carries no words to read. It waits on an overload of its own.
//
// xstd::uint128 IS here, and that is a property of how the tests are built rather than of the library: it is
// a class on MSVC and carries its own overloads, while on GCC and Clang it is the builtin, which reaches
// std::unsigned_integral only outside __STRICT_ANSI__. test/src/ints/bit.cpp asserts that it has a basis, so
// turning CMAKE_CXX_EXTENSIONS back off says so there rather than here, at every instantiation list at once.
template<class T>
concept has_popcount = requires (T x) { xstd::popcount(x); };

using worded_unsigned_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>()));

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(CountlZero)

// Where <bit> answers, this IS <bit>: the overload forwards and must not drift from it.
BOOST_AUTO_TEST_CASE(AgreesWithStdWhereStdAnswers)
{
        XSTD_CONSTEXPR_CHECK(xstd::countl_zero(std::uint64_t{0}) == std::countl_zero(std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::countl_zero(std::uint64_t{1}) == std::countl_zero(std::uint64_t{1}));
        XSTD_CONSTEXPR_CHECK(xstd::countl_zero(~std::uint64_t{0}) == std::countl_zero(~std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::countl_zero(std::uint64_t{1} << 63) == std::countl_zero(std::uint64_t{1} << 63));
}

// Total across every width the library carries, the 128-bit classes included -- which is where this reaches
// past <bit>, since MSVC's std::_Unsigned128, absl::uint128 and boost::int128::uint128 all decline it.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsOfEveryWidth, T, worded_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;
        static_assert(xstd::countl_zero(T{0}) == W);
        static_assert(xstd::countl_zero(T{1}) == W - 1);
        static_assert(xstd::countl_zero(xstd::numeric_limits<T>::max()) == 0);
        BOOST_CHECK(true);
}

// The word boundary, which is what pins each class's low half to its low half: read through the wrong
// accessor, a type answers this with the halves swapped, so it fails rather than merely looking odd.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheWordBoundary, T, worded_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;
        if constexpr (W >= 128) {
                constexpr auto bit64 = static_cast<T>(T{1} << 64);
                static_assert(xstd::countl_zero(bit64) == W - 65);
        }
        BOOST_CHECK(true);
}

// The constraint is exactly what the body needs, not a hair wider. std::unsigned_integral would have been
// wider: it admits bool and the three unicode character types, which <bit> refuses, so an overload
// constrained on it advertises four types it cannot serve and hard-errors inside instead of not matching.
// Asserted as an equality so it fails just as loudly if this ever became NARROWER than <bit>.
namespace {

template<class T>
concept xstd_answers = requires (T x) { xstd::countl_zero(x); };

template<class T>
concept std_answers = requires (T x) { std::countl_zero(x); };

} // namespace

BOOST_AUTO_TEST_CASE(TheConstraintIsTheBodys)
{
        static_assert(xstd_answers<bool> == std_answers<bool>);
        static_assert(xstd_answers<char8_t> == std_answers<char8_t>);
        static_assert(xstd_answers<char16_t> == std_answers<char16_t>);
        static_assert(xstd_answers<char32_t> == std_answers<char32_t>);
        static_assert(xstd_answers<unsigned char> == std_answers<unsigned char>);
        static_assert(xstd_answers<std::uint64_t> == std_answers<std::uint64_t>);

        // And those first four really are the gap std::unsigned_integral would have opened.
        static_assert(std::unsigned_integral<bool> and not std_answers<bool>);
        static_assert(std::unsigned_integral<char8_t> and not std_answers<char8_t>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
