//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_ceil.hpp> // bit_ceil
#include <xstd/ints/cstdint.hpp>      // XSTD_HAS_BIT_INT, XSTD_HAS_INT128, bit_int, bit_uint, int128, uint128
#include <xstd/ints/limits.hpp>       // numeric_limits
#include <test/bit/oracle.hpp>        // oracle_bit_ceil
#include <test/bit/sweep.hpp>         // bit_precise_sweep_types, for_each_edge_value_at, for_each_sweep_value, holds_at_every_position
#include <test/constexpr_check.hpp>   // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp> // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>   // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <bit>                        // bit_ceil
#include <cstdint>                    // int32_t, uint64_t
#include <tuple>                      // tuple_cat
#include <type_traits>                // integral_constant
#include <utility>                    // declval, make_integer_sequence

namespace {

using basis_unsigned_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>(),
        std::declval<test::bit_precise_sweep_types>()
));

template<class T>
concept xstd_answers = requires (T x) { xstd::bit_ceil(x); };

template<class T>
concept std_answers = requires (T x) { std::bit_ceil(x); };

// The top bit, past which no ceiling is representable.
template<class T>
constexpr auto top_bit = static_cast<T>(xstd::numeric_limits<T>::max() ^ static_cast<T>(xstd::numeric_limits<T>::max() >> 1U));

// The precondition's domain: every value up to and including the top bit.
template<class T>
constexpr auto in_domain(T x)
        -> bool
{
        return x <= top_bit<T>;
}

// Whether the call is a constant expression, asked as a template argument so that a refusal is false, not an error.
template<class T, int Past>
concept ceils_as_a_constant = requires { typename std::integral_constant<bool, xstd::bit_ceil(static_cast<T>(top_bit<T> + static_cast<T>(Past))) != T{0}>; };

template<class T>
[[nodiscard]] constexpr auto agrees_at(int k)
        -> bool
{
        auto agrees = true;
        test::for_each_edge_value_at<T>(k, [&](T x) -> void { agrees = agrees and (not in_domain(x) or xstd::bit_ceil(x) == test::oracle_bit_ceil(x)); });
        return agrees;
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(BitCeil)

// Where <bit> answers, this IS <bit>: the overload forwards and must not drift from it.
BOOST_AUTO_TEST_CASE(AgreesWithStdWhereStdAnswers)
{
        XSTD_CONSTEXPR_CHECK(xstd::bit_ceil(std::uint64_t{0}) == std::bit_ceil(std::uint64_t{0}));
        XSTD_CONSTEXPR_CHECK(xstd::bit_ceil(std::uint64_t{1}) == std::bit_ceil(std::uint64_t{1}));
        XSTD_CONSTEXPR_CHECK(xstd::bit_ceil(std::uint64_t{1000}) == std::bit_ceil(std::uint64_t{1000}));
        XSTD_CONSTEXPR_CHECK(xstd::bit_ceil(std::uint64_t{1} << 63U) == std::bit_ceil(std::uint64_t{1} << 63U));
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

// A narrow contract is not noexcept, whatever the type.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsNotNoexcept, T, basis_unsigned_types)
{
        static_assert(not noexcept(xstd::bit_ceil(T{1})));
        BOOST_CHECK(true);
}

// [bit.pow.two]'s remark: a value past the top bit is no constant expression, with or without NDEBUG.
BOOST_AUTO_TEST_CASE_TEMPLATE(AValuePastTheTopBitIsNoConstant, T, basis_unsigned_types)
{
        static_assert(ceils_as_a_constant<T, 0>);
        static_assert(not ceils_as_a_constant<T, 1>);
        BOOST_CHECK(true);
}

// Zero and one both round up to one, and everything above half the top bit rounds up to the top bit.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsOfEveryWidth, T, basis_unsigned_types)
{
        constexpr auto ones = xstd::numeric_limits<T>::max();
        constexpr auto top  = static_cast<T>(ones ^ static_cast<T>(ones >> 1U));
        static_assert(xstd::bit_ceil(T{0}) == T{1});
        static_assert(xstd::bit_ceil(T{1}) == T{1});
        static_assert(xstd::bit_ceil(top) == top);
        static_assert(xstd::bit_ceil(static_cast<T>((top >> 1U) + T{1})) == top);
        BOOST_CHECK(true);
}

// Each width against a doubling oracle within the precondition: edges at compile time, a sweep at run time.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheOracle, T, basis_unsigned_types)
{
        static_assert(test::holds_at_every_position<agrees_at<T>>(std::make_integer_sequence<int, xstd::numeric_limits<T>::digits>{}));
        auto mismatches = 0UZ;
        test::for_each_sweep_value<T>([&](T value) -> void {
                // Halved, every value is below the top bit: no call outside the domain is left to hoist.
                auto const& x = static_cast<T>(value >> 1U);
                mismatches += xstd::bit_ceil(x) == test::oracle_bit_ceil(x) ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
