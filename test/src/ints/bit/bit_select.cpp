//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_select.hpp> // bit_select
#include <xstd/ints/bit/popcount.hpp>   // popcount
#include <xstd/ints/cstdint.hpp>        // XSTD_HAS_INT128, bit_int, bit_uint, uint128
#include <xstd/ints/limits.hpp>         // numeric_limits
#include <test/bit/oracle.hpp>          // oracle_bit_select
#include <test/bit/sweep.hpp>           // alternating_ones, bit_precise_sweep_types, for_each_edge_value_at, for_each_sweep_value, holds_at_every_position, width
#include <test/constexpr_check.hpp>     // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>   // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <climits>                      // INT_MAX, INT_MIN
#include <cstdint>                      // int32_t, uint32_t, uint64_t
#include <ranges>                       // iota
#include <tuple>                        // tuple_cat
#include <type_traits>                  // integral_constant
#include <utility>                      // declval, make_integer_sequence

namespace {

using selectable_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>(),
        std::declval<test::bit_precise_sweep_types>()
));

template<class T>
concept xstd_answers = requires (T x) { xstd::bit_select(x, 0); };

// Whether the call is a constant expression, asked as a template argument so that a refusal is false, not an error.
template<std::uint32_t X, int K>
concept selects_as_a_constant = requires { typename std::integral_constant<int, xstd::bit_select(X, K)>; };

// Walking x one bit at a time, each one met is the one bit_select finds at the rank it is met with.
template<class T>
constexpr auto selects_every_one(T x)
        -> bool
{
        auto agrees = true;
        auto rank   = 0;
        auto rest   = x;
        for (auto const n : std::views::iota(0, xstd::numeric_limits<T>::digits)) {
                if (static_cast<T>(rest & T{1}) != T{0}) {
                        agrees = agrees and xstd::bit_select(x, rank) == n;
                        ++rank;
                }
                rest = static_cast<T>(rest >> 1U);
        }
        return agrees;
}

template<class T>
constexpr auto agrees_at(int k)
        -> bool
{
        auto agrees = true;
        test::for_each_edge_value_at<T>(k, [&](T x) -> void { agrees = agrees and selects_every_one(x); });
        return agrees;
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(BitSelect)

// Ranks count from zero at the least significant one.
BOOST_AUTO_TEST_CASE(AnExample)
{
        XSTD_CONSTEXPR_CHECK(xstd::bit_select(std::uint32_t{0b1011'0100U}, 0) == 2);
        XSTD_CONSTEXPR_CHECK(xstd::bit_select(std::uint32_t{0b1011'0100U}, 1) == 4);
        XSTD_CONSTEXPR_CHECK(xstd::bit_select(std::uint32_t{0b1011'0100U}, 2) == 5);
        XSTD_CONSTEXPR_CHECK(xstd::bit_select(std::uint32_t{0b1011'0100U}, 3) == 7);
}

// An unsigned integer type and nothing else, as <bit>'s functions ask, widened to the unsigned integers xstd knows.
BOOST_AUTO_TEST_CASE(TheConstraintIsUnsignedInteger)
{
        static_assert(xstd_answers<unsigned char>);
        static_assert(xstd_answers<std::uint64_t>);
#ifdef XSTD_HAS_INT128
        static_assert(xstd_answers<xstd::uint128>);
#endif
        static_assert(not xstd_answers<bool>);
        static_assert(not xstd_answers<char8_t>);
        static_assert(not xstd_answers<char32_t>);
        static_assert(not xstd_answers<std::int32_t>);
#ifdef XSTD_HAS_BIT_INT
        static_assert(xstd_answers<xstd::bit_uint<24>>);
        static_assert(not xstd_answers<xstd::bit_int<24>>);
#endif
        BOOST_CHECK(true);
}

// A narrow contract is not noexcept, whatever the type.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsNotNoexcept, T, selectable_types)
{
        static_assert(not noexcept(xstd::bit_select(T{1}, 0)));
        BOOST_CHECK(true);
}

// A rank that is negative or not below the population is no constant expression, with or without NDEBUG.
BOOST_AUTO_TEST_CASE(ARankOutsideThePopulationIsNoConstant)
{
        static_assert(selects_as_a_constant<0b1011'0100U, 0>);
        static_assert(selects_as_a_constant<0b1011'0100U, 3>);
        static_assert(not selects_as_a_constant<0b1011'0100U, 4>);
        static_assert(not selects_as_a_constant<0b1011'0100U, -1>);
        static_assert(not selects_as_a_constant<0b1011'0100U, INT_MIN>);
        static_assert(not selects_as_a_constant<0b1011'0100U, INT_MAX>);
        static_assert(not selects_as_a_constant<0U, 0>);
        BOOST_CHECK(true);
}

// The lowest and highest bits, alone and together, every rank of a full value, and ones on either side of bit 64.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheExtremeValues, T, selectable_types)
{
        constexpr auto N    = xstd::numeric_limits<T>::digits;
        constexpr auto ones = xstd::numeric_limits<T>::max();
        constexpr auto top  = static_cast<T>(T{1} << unsigned{N - 1});
        static_assert(xstd::bit_select(T{1}, 0) == 0);
        static_assert(xstd::bit_select(top, 0) == N - 1);
        static_assert(xstd::bit_select(ones, 0) == 0);
        static_assert(xstd::bit_select(ones, N - 1) == N - 1);
        static_assert(xstd::bit_select(static_cast<T>(top | T{1}), 1) == N - 1);
        if constexpr (test::width<T> > 64UZ) {
                constexpr auto straddle = static_cast<T>(static_cast<T>(T{1} << 63U) | static_cast<T>(T{1} << 64U));
                static_assert(xstd::bit_select(straddle, 0) == 63);
                static_assert(xstd::bit_select(straddle, 1) == 64);
        }
        auto mismatches = 0UZ;
        for (auto const k : std::views::iota(0, N)) {
                mismatches += xstd::bit_select(ones, k) == k ? 0UZ : 1UZ;
        }
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// Each width against a bit-at-a-time oracle: every rank of its edges at compile time, of a sweep at run time.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheOracle, T, selectable_types)
{
        static_assert(selects_every_one(xstd::numeric_limits<T>::max()) and selects_every_one(test::alternating_ones<T>()));
        static_assert(test::holds_at_every_position<agrees_at<T>>(std::make_integer_sequence<int, xstd::numeric_limits<T>::digits>{}));
        auto mismatches = 0UZ;
        test::for_each_sweep_value<T>([&](T x) -> void {
                for (auto const k : std::views::iota(0, xstd::popcount(x))) {
                        mismatches += xstd::bit_select(x, k) == test::oracle_bit_select(x, k) ? 0UZ : 1UZ;
                }
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
