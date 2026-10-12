//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit.hpp>                   // complete bit basis
#include <xstd/ints/cstdint.hpp>               // XSTD_HAS_INT128, uint128
#include <xstd/ints/limits.hpp>                // numeric_limits
#include <test/bit/permutation_properties.hpp> // low_ones, round_trips
#include <test/bit/shift_count.hpp>            // shift_count_t
#include <test/bit/sweep.hpp>                  // bit_precise_sweep_types, for_each_edge_value, for_each_sweep_pair, for_each_sweep_value
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>          // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>            // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <cstdint>                             // uint32_t
#include <tuple>                               // tuple_cat
#include <utility>                             // declval

namespace {

// Every exact width, the unsigned bit-precise ones reaching <bit> where it takes them and a builtin where it does not.
template<class T>
concept has_popcount = requires (T x) { xstd::popcount(x); };

using basis_unsigned_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>(),
        std::declval<test::bit_precise_sweep_types>()
));

// P3104R5's countr_zero, its alternating masks named by bit_repeat rather than spelled as magic numbers.
constexpr auto countr_zero_by_repeat(std::uint32_t v)
        -> int
{
        auto c = 32;
        v &= static_cast<std::uint32_t>(~v + 1U);
        if (v != 0U) {
                --c;
        }
        for (auto i = 16; i != 0; i /= 2) {
                auto const mask = xstd::bit_repeat(static_cast<std::uint32_t>((1U << static_cast<unsigned>(i)) - 1U), i * 2);
                if ((v & mask) != 0U) {
                        c -= i;
                }
        }
        return c;
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)

// The three together cross-check the halves: swapped accessors keep each single-function identity and break the sum.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheThreeAgreeOnASingleBit, T, basis_unsigned_types)
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

// The widest exact width has a basis on every configured leg, through <bit> or a block at a time.
BOOST_AUTO_TEST_CASE(TheWidestExactWidthHasABasis)
{
#ifdef XSTD_HAS_INT128
        static_assert(has_popcount<xstd::uint128>);
        static_assert(xstd::popcount(xstd::uint128{0}) == 0);
#endif
        BOOST_CHECK(true);
}

// The functions agree with each other: each one-count is a zero-count of the complement, and the width bounds the floor.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheFunctionsAgree, T, basis_unsigned_types)
{
        constexpr auto W = xstd::numeric_limits<T>::digits;
        auto mismatches  = 0UZ;
        test::for_each_sweep_value<T>([&](T x) -> void {
                mismatches += xstd::bit_width(x) == W - xstd::countl_zero(x) ? 0UZ : 1UZ;
                mismatches += xstd::countl_one(x) == xstd::countl_zero(static_cast<T>(~x)) ? 0UZ : 1UZ;
                mismatches += xstd::countr_one(x) == xstd::countr_zero(static_cast<T>(~x)) ? 0UZ : 1UZ;
                mismatches += xstd::has_single_bit(x) == (xstd::popcount(x) == 1) ? 0UZ : 1UZ;
                mismatches += x == T{0} or (xstd::has_single_bit(xstd::bit_floor(x)) and xstd::bit_width(xstd::bit_floor(x)) == xstd::bit_width(x)) ? 0UZ : 1UZ;
                mismatches += xstd::rotl(x, 1) == xstd::rotr(x, -1) and xstd::rotl(x, -W) == x ? 0UZ : 1UZ;
                mismatches += xstd::popcount(xstd::rotl(x, 3)) == xstd::popcount(x) ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// Compressing and expanding through one mask undo each other, as far as the mask's population lets them.
BOOST_AUTO_TEST_CASE_TEMPLATE(CompressAndExpandAreInverse, T, basis_unsigned_types)
{
        auto mismatches     = 0UZ;
        auto const compress = [](T x, T m) -> T { return xstd::bit_compress(x, m); };
        auto const expand   = [](T x, T m) -> T { return xstd::bit_expand(x, m); };
        test::for_each_sweep_pair<T>([&](T x, T m) -> void { mismatches += test::round_trips(x, m, compress, expand) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// Compression from the top is reversal around compression from the bottom, which pins the blocks to their mirror images.
BOOST_AUTO_TEST_CASE_TEMPLATE(ReversalMirrorsCompression, T, basis_unsigned_types)
{
        auto mismatches = 0UZ;
        test::for_each_sweep_pair<T>([&](T x, T m) -> void {
                // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const unselected = xstd::numeric_limits<T>::digits - xstd::popcount(m);
                auto const& from_top  = xstd::bit_reverse(xstd::bit_compress(xstd::bit_reverse(x), xstd::bit_reverse(m)));
                auto const& expected  = unselected < xstd::numeric_limits<T>::digits ? static_cast<T>(xstd::bit_compress(x, m) << static_cast<test::shift_count_t<T>>(unselected)) : T{0};
                mismatches += from_top == expected ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// P3104R5's popcount equivalences, each counting the trailing ones as the trailing zeros of the complement.
BOOST_AUTO_TEST_CASE_TEMPLATE(CompressionCountsThePopulation, T, basis_unsigned_types)
{
        auto mismatches = 0UZ;
        test::for_each_edge_value<T>([&](T x) -> void {
                auto const ones = xstd::numeric_limits<T>::max();
                mismatches += xstd::countr_zero(static_cast<T>(~xstd::bit_compress(ones, x))) == xstd::popcount(x) ? 0UZ : 1UZ;
                mismatches += xstd::countr_zero(static_cast<T>(~xstd::bit_compress(x, x))) == xstd::popcount(x) ? 0UZ : 1UZ;
                mismatches += xstd::bit_compress(x, x) == test::low_ones<T>(xstd::popcount(x)) ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// P3104R5's interleaving: x into the odd positions and y into the even ones gives the Z-order index.
BOOST_AUTO_TEST_CASE(InterleavingByExpansion)
{
        constexpr auto x = 3U;
        constexpr auto y = 5U;
        XSTD_CONSTEXPR_CHECK((xstd::bit_expand(x, xstd::bit_repeat(0b10U, 2)) | xstd::bit_expand(y, xstd::bit_repeat(0b01U, 2))) == 0b01'10'11U);
}

// And the interleaving undone: compressing through either mask gives back its coordinate.
BOOST_AUTO_TEST_CASE_TEMPLATE(InterleavingRoundTrips, T, basis_unsigned_types)
{
        if constexpr (xstd::numeric_limits<T>::digits >= 2) {
                constexpr auto odd  = xstd::bit_repeat(static_cast<T>(0b10U), 2);
                constexpr auto even = xstd::bit_repeat(static_cast<T>(0b01U), 2);
                auto mismatches     = 0UZ;
                test::for_each_sweep_pair<T>([&](T x, T y) -> void {
                        // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                        auto const& z = static_cast<T>(xstd::bit_expand(x, odd) | xstd::bit_expand(y, even));
                        mismatches += xstd::bit_compress(z, odd) == static_cast<T>(x & test::low_ones<T>(xstd::numeric_limits<T>::digits / 2)) ? 0UZ : 1UZ;
                        mismatches += xstd::bit_compress(z, even) == static_cast<T>(y & test::low_ones<T>((xstd::numeric_limits<T>::digits + 1) / 2)) ? 0UZ : 1UZ;
                });
                BOOST_CHECK_EQUAL(mismatches, 0UZ);
        } else {
                BOOST_CHECK(true);
        }
}

// P3104R5's countr_zero by bit_repeat agrees with countr_zero itself.
BOOST_AUTO_TEST_CASE(CountingTrailingZerosByRepetition)
{
        auto mismatches = 0UZ;
        test::for_each_edge_value<std::uint32_t>([&](std::uint32_t v) -> void { mismatches += countr_zero_by_repeat(v) == xstd::countr_zero(v) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
        static_assert(countr_zero_by_repeat(0U) == 32);
        static_assert(countr_zero_by_repeat(0x8000'0000U) == 31);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
