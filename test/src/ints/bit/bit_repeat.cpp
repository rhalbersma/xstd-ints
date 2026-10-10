//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_repeat.hpp> // bit_repeat
#include <xstd/ints/cstdint.hpp>        // bit_int, bit_uint, uint128
#include <xstd/ints/limits.hpp>         // numeric_limits
#include <test/bit_reference.hpp>       // bit_precise_sweep_types, for_each_edge_value_at, for_each_sweep_value, holds_at_every_position, reference_bit_repeat
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

using permutable_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>(),
        std::declval<test::bit_precise_sweep_types>()
));

template<class T>
concept xstd_answers = requires (T x) { xstd::bit_repeat(x, 1); };

// Whether the call is a constant expression, asked as a template argument so that a refusal is false, not an error.
template<int L>
concept repeats_as_a_constant = requires { typename std::integral_constant<std::uint32_t, xstd::bit_repeat(std::uint32_t{1}, L)>; };

template<class T>
constexpr auto agrees_at(int k)
        -> bool
{
        auto agrees      = true;
        constexpr auto N = xstd::numeric_limits<T>::digits;
        for (auto const l : {1, 3, N - 1, N}) {
                test::for_each_edge_value_at<T>(k, [&](T x) -> void { agrees = agrees and xstd::bit_repeat(x, l) == test::reference_bit_repeat(x, l); });
        }
        return agrees;
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(BitRepeat)

// P3104R5's example, and the masks of its countr_zero example, spelled unsigned so that the constraint admits them.
BOOST_AUTO_TEST_CASE(ThePapersExamples)
{
        XSTD_CONSTEXPR_CHECK(xstd::bit_repeat(std::uint32_t{0xc}, 4) == std::uint32_t{0xcccc'cccc});
        XSTD_CONSTEXPR_CHECK(xstd::bit_repeat((1U << 16U) - 1U, 32) == 0x0000'ffffU);
        XSTD_CONSTEXPR_CHECK(xstd::bit_repeat((1U << 8U) - 1U, 16) == 0x00ff'00ffU);
        XSTD_CONSTEXPR_CHECK(xstd::bit_repeat((1U << 4U) - 1U, 8) == 0x0f0f'0f0fU);
        XSTD_CONSTEXPR_CHECK(xstd::bit_repeat((1U << 2U) - 1U, 4) == 0x3333'3333U);
        XSTD_CONSTEXPR_CHECK(xstd::bit_repeat((1U << 1U) - 1U, 2) == 0x5555'5555U);
}

// An unsigned integer type and nothing else, as [bit.permute] asks, widened to the unsigned integers xstd knows.
BOOST_AUTO_TEST_CASE(TheConstraintIsUnsignedInteger)
{
        static_assert(xstd_answers<unsigned char>);
        static_assert(xstd_answers<std::uint64_t>);
        static_assert(xstd_answers<xstd::uint128>);
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
BOOST_AUTO_TEST_CASE_TEMPLATE(IsNotNoexcept, T, permutable_types)
{
        static_assert(not noexcept(xstd::bit_repeat(T{1}, 1)));
        BOOST_CHECK(true);
}

// P3104R6's remark: a length that is not positive is no constant expression, with or without NDEBUG.
BOOST_AUTO_TEST_CASE(ALengthBelowOneIsNoConstant)
{
        static_assert(repeats_as_a_constant<1>);
        static_assert(repeats_as_a_constant<INT_MAX>);
        static_assert(not repeats_as_a_constant<0>);
        static_assert(not repeats_as_a_constant<-1>);
        static_assert(not repeats_as_a_constant<INT_MIN>);
        BOOST_CHECK(true);
}

// A period of one copies the lowest bit everywhere, and one as wide as the type or wider leaves the value as it is.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheExtremePeriods, T, permutable_types)
{
        constexpr auto N    = xstd::numeric_limits<T>::digits;
        constexpr auto ones = xstd::numeric_limits<T>::max();
        constexpr auto high = static_cast<T>(ones - T{1});
        static_assert(xstd::bit_repeat(T{1}, 1) == ones);
        static_assert(xstd::bit_repeat(high, 1) == T{0});
        static_assert(xstd::bit_repeat(high, N) == high);
        static_assert(xstd::bit_repeat(high, N + 1) == high);
        static_assert(xstd::bit_repeat(high, INT_MAX) == high);
        BOOST_CHECK(true);
}

// Each width against a bit-at-a-time reference: its edges at compile time, every period over a sweep at run time.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheReference, T, permutable_types)
{
        static_assert(test::holds_at_every_position<agrees_at<T>>(std::make_integer_sequence<int, xstd::numeric_limits<T>::digits>{}));
        auto mismatches = 0UZ;
        for (auto const l : std::views::iota(1, xstd::numeric_limits<T>::digits + 2)) {
                test::for_each_sweep_value<T>([&](T x) -> void { mismatches += xstd::bit_repeat(x, l) == test::reference_bit_repeat(x, l) ? 0UZ : 1UZ; });
        }
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
