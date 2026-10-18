//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/has_single_bit.hpp>               // has_single_bit
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdint.hpp>                          // XSTD_HAS_INT128, bit_int, bit_uint, int128, uint128
#include <xstd/ints/limits.hpp>                           // numeric_limits
#include <test/bit/oracle.hpp>                            // oracle_has_single_bit
#include <test/bit/sweep.hpp>                             // bit_precise_sweep_types, for_each_edge_value_at, for_each_sweep_value, holds_at_every_position
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>                     // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <bit>                                            // has_single_bit
#include <cstdint>                                        // int32_t, uint64_t
#include <tuple>                                          // tuple_cat
#include <utility>                                        // declval, make_integer_sequence

namespace {

using basis_unsigned_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>(),
        std::declval<test::bit_precise_sweep_types>()
));

template<class T>
concept xstd_answers = requires (T x) { xstd::has_single_bit(x); };

template<class T>
concept std_answers = requires (T x) { std::has_single_bit(x); };

template<class T>
[[nodiscard]] constexpr auto agrees_at(int k)
        -> bool
{
        auto agrees = true;
        test::for_each_edge_value_at<T>(k, [&](T x) -> void { agrees = agrees and xstd::has_single_bit(x) == test::oracle_has_single_bit(x); });
        return agrees;
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(HasSingleBit)

// Where <bit> answers, this IS <bit>: the overload forwards and must not drift from it.
BOOST_AUTO_TEST_CASE(AgreesWithStdWhereStdAnswers)
{
        XSTD_CONSTEXPR_CHECK(xstd::has_single_bit(std::uint64_t{0}) == std::has_single_bit(std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::has_single_bit(std::uint64_t{1}) == std::has_single_bit(std::uint64_t{1}));
        XSTD_CONSTEXPR_CHECK(xstd::has_single_bit(~std::uint64_t{0}) == std::has_single_bit(~std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::has_single_bit(std::uint64_t{1} << 63U) == std::has_single_bit(std::uint64_t{1} << 63U));
}

// <bit>'s own answer wherever it gives one, widened to the unsigned integers it declines and to nothing else.
BOOST_AUTO_TEST_CASE(TheConstraintIsUnsignedInteger)
{
        static_assert(xstd_answers<bool> == std_answers<bool>);
        static_assert(xstd_answers<char8_t> == std_answers<char8_t>);
        static_assert(xstd_answers<char32_t> == std_answers<char32_t>);
        static_assert(xstd_answers<unsigned char> == std_answers<unsigned char>);
        static_assert(xstd_answers<std::uint64_t> == std_answers<std::uint64_t>);
        static_assert(not xstd_answers<std::int32_t>);
#ifdef XSTD_HAS_INT128
        static_assert(xstd_answers<xstd::uint128>);
        static_assert(not xstd_answers<xstd::int128>);
#endif
#ifdef XSTD_HAS_BIT_INT
        static_assert(xstd_answers<xstd::bit_uint<24>>);
        static_assert(not xstd_answers<xstd::bit_int<24>>);
#endif
        BOOST_CHECK(true);
}

// Unconditional where the operations cannot throw, and otherwise as conditional as the type's own operators.
BOOST_AUTO_TEST_CASE_TEMPLATE(NoexceptFollowsTheOperators, T, basis_unsigned_types)
{
        static_assert(noexcept(xstd::has_single_bit(T{1})) == xstd::nothrow_const_operators<T>);
        BOOST_CHECK(true);
}

// Total across every width the library carries, the 128-bit classes included, which is where this reaches past <bit>.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsOfEveryWidth, T, basis_unsigned_types)
{
        constexpr auto ones = xstd::numeric_limits<T>::max();
        static_assert(not xstd::has_single_bit(T{0}));
        static_assert(xstd::has_single_bit(T{1}));
        static_assert(xstd::has_single_bit(static_cast<T>(ones ^ static_cast<T>(ones >> 1U))));
        static_assert(xstd::numeric_limits<T>::digits == 1 or not xstd::has_single_bit(ones));
        BOOST_CHECK(true);
}

// Each width against a bit-at-a-time oracle: its edges at compile time, every value or a sweep at run time.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheOracle, T, basis_unsigned_types)
{
        static_assert(test::holds_at_every_position<agrees_at<T>>(std::make_integer_sequence<int, xstd::numeric_limits<T>::digits>{}));
        auto mismatches = 0UZ;
        test::for_each_sweep_value<T>([&](T x) -> void { mismatches += xstd::has_single_bit(x) == test::oracle_has_single_bit(x) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
