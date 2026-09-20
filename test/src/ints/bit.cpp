//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit.hpp>          // complete bit basis
#include <xstd/ints/cstdint.hpp>      // uint128
#include <xstd/ints/limits.hpp>       // numeric_limits
#include <test/exact_width_types.hpp> // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>   // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <tuple>                      // tuple_cat
#include <utility>                    // declval

namespace {

// Every exact width but the bit-precise: unsigned _BitInt(N) reaches neither <bit> nor any overload here.
template<class T>
concept has_popcount = requires (T x) { xstd::popcount(x); };

using worded_unsigned_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>()
));

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)

// The three together cross-check the halves: swapped accessors keep each single-function identity and break the sum.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheThreeAgreeOnASingleBit, T, worded_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;

        // For a one-bit value the leading and trailing runs must partition the width exactly.
        static_assert(xstd::countl_zero(T{1}) + xstd::countr_zero(T{1}) == W - 1);
        static_assert(xstd::popcount(T{1}) == 1);

        if constexpr (W >= 128) {
                constexpr auto bit64 = static_cast<T>(T{1} << 64U);
                static_assert(xstd::countl_zero(bit64) + xstd::countr_zero(bit64) == W - 1);
                static_assert(xstd::popcount(bit64) == 1);

                // Both halves set: each scan stops in its own half, which swapped accessors cannot fake.
                constexpr auto both = static_cast<T>(bit64 | T{1});
                static_assert(xstd::popcount(both) == 2);
                static_assert(xstd::countr_zero(both) == 0);
                static_assert(xstd::countl_zero(both) == W - 65);
        }

        // A saturated value leaves no zeros on either end, and the population is the width.
        static_assert(xstd::countl_zero(xstd::numeric_limits<T>::max()) == 0);
        static_assert(xstd::countr_zero(xstd::numeric_limits<T>::max()) == 0);
        static_assert(xstd::popcount(xstd::numeric_limits<T>::max()) == W);

        // And an empty value is all zeros on both ends.
        static_assert(xstd::countl_zero(T{0}) == W);
        static_assert(xstd::countr_zero(T{0}) == W);
        static_assert(xstd::popcount(T{0}) == 0);

        BOOST_CHECK(true);
}

// The widest exact width has a basis on every configured leg, which is what CMAKE_CXX_EXTENSIONS ON buys.
BOOST_AUTO_TEST_CASE(TheWidestExactWidthHasABasis)
{
        static_assert(has_popcount<xstd::uint128>);
        static_assert(xstd::popcount(xstd::uint128{0}) == 0);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
