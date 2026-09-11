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
#include <bit>                           // countr_zero
#include <cstdint>                       // uint64_t
#include <tuple>                         // tuple, tuple_cat
#include <type_traits>                   // conditional_t
#include <utility>                       // declval

namespace {

// The exact-width list MINUS the bit-precise types: unsigned _BitInt(N) reaches neither <bit> -- libstdc++
// constrains these functions to the five standard unsigned types by is_same -- nor any overload here, and
// unlike the 128-bit classes it carries no words to read. It waits on an overload of its own.
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
BOOST_AUTO_TEST_SUITE(CountrZero)

// Where <bit> answers, this IS <bit>: the overload forwards and must not drift from it.
BOOST_AUTO_TEST_CASE(AgreesWithStdWhereStdAnswers)
{
        XSTD_CONSTEXPR_CHECK(xstd::countr_zero(std::uint64_t{0}) == std::countr_zero(std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::countr_zero(std::uint64_t{1}) == std::countr_zero(std::uint64_t{1}));
        XSTD_CONSTEXPR_CHECK(xstd::countr_zero(~std::uint64_t{0}) == std::countr_zero(~std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::countr_zero(std::uint64_t{1} << 63) == std::countr_zero(std::uint64_t{1} << 63));
}

// Total across every width the library carries, the 128-bit classes included -- which is where this reaches
// past <bit>, since MSVC's std::_Unsigned128, absl::uint128 and boost::int128::uint128 all decline it.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsOfEveryWidth, T, worded_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;
        static_assert(xstd::countr_zero(T{0}) == (W));
        static_assert(xstd::countr_zero(T{1}) == (0));
        static_assert(xstd::countr_zero(xstd::numeric_limits<T>::max()) == (0));
        BOOST_CHECK(true);
}

// The word boundary, which is what pins each class's low half to its low half: read through the wrong
// accessor, a type answers this with the halves swapped, so it fails rather than merely looking odd.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheWordBoundary, T, worded_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;
        if constexpr (W >= 128) {
                constexpr auto bit64 = static_cast<T>(T{1} << 64);
                static_assert(xstd::countr_zero(bit64) == (64));
        }
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
