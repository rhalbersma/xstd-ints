//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit.hpp>          // complete bit basis
#include <xstd/ints/cstdint.hpp>      // uint128
#include <xstd/ints/limits.hpp>       // numeric_limits
#include <test/exact_width_types.hpp> // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>   // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <tuple>                      // tuple, tuple_cat
#include <type_traits>                // conditional_t
#include <utility>                    // declval

namespace {

// See bit/popcount.cpp: unsigned _BitInt(N) is absent from this list because nothing answers for it yet.
// xstd::uint128 is a class on MSVC and carries its own overloads; on GCC and Clang it is the builtin, which
// reaches <bit> only outside __STRICT_ANSI__. The condition worth testing is the overload's existence, which
// no #if can spell, so the column is included exactly when something answers for it and test/src/ints/bit.cpp
// holds the macro to that in both directions. [xstd-bits design.md#uint128-support]
template<class T>
concept has_popcount = requires (T x) { xstd::popcount(x); };

inline constexpr auto uint128_has_basis = has_popcount<xstd::uint128>;
using uint128_types = std::conditional_t<uint128_has_basis, test::xstd_unsigned_types, std::tuple<>>;

using worded_unsigned_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<uint128_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>()));

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)

// The three together, which is the only place the halves can be cross-checked against each other: a type read
// through swapped accessors keeps every single-function identity below and breaks the sum.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheThreeAgreeOnASingleBit, T, worded_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;

        // For a one-bit value the leading and trailing runs must partition the width exactly.
        static_assert(xstd::countl_zero(T{1}) + xstd::countr_zero(T{1}) == W - 1);
        static_assert(xstd::popcount(T{1}) == 1);

        if constexpr (W >= 128) {
                constexpr auto bit64 = static_cast<T>(T{1} << 64);
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

// The macro held to the concept in both directions, so the day the seam grows an overload of its own -- or a
// new pairing lands on the matrix -- the build says so here rather than at a dozen instantiation lists or,
// worse, nowhere. [xstd-bits design.md#uint128-support]
BOOST_AUTO_TEST_CASE(TheBuiltInColumnTracksTheMode)
{
#ifdef _MSC_VER
        // std::_Unsigned128 is a class and carries its own overloads, in every mode.
        static_assert(uint128_has_basis);
#elifdef __STRICT_ANSI__
        // unsigned __int128 reaches std::unsigned_integral only outside __STRICT_ANSI__, and nothing else answers.
        static_assert(not uint128_has_basis);
#else
        static_assert(uint128_has_basis);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
