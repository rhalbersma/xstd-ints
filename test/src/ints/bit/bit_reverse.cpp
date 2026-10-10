//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_reverse.hpp>                  // bit_reverse
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdint.hpp>                          // bit_int, bit_uint, uint128
#include <xstd/ints/limits.hpp>                           // numeric_limits
#include <test/bit_reference.hpp>                         // bit_precise_sweep_types, for_each_edge_value_at, for_each_sweep_value, holds_at_every_position, reference_bit_reverse
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>                     // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <cstdint>                                        // int32_t, uint32_t, uint64_t
#include <tuple>                                          // tuple_cat
#include <utility>                                        // declval, make_integer_sequence

namespace {

using permutable_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>(),
        std::declval<test::bit_precise_sweep_types>()
));

template<class T>
concept xstd_answers = requires (T x) { xstd::bit_reverse(x); };

template<class T>
constexpr auto agrees_at(int k)
        -> bool
{
        auto agrees = true;
        test::for_each_edge_value_at<T>(k, [&](T x) -> void { agrees = agrees and xstd::bit_reverse(x) == test::reference_bit_reverse(x); });
        return agrees;
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(BitReverse)

// P3104R5's example, whose printed 0x24c80000 transposes two digits of the mirror image 0x2c480000.
BOOST_AUTO_TEST_CASE(ThePapersExample)
{
        XSTD_CONSTEXPR_CHECK(xstd::bit_reverse(std::uint32_t{0x0000'1234}) == std::uint32_t{0x2c48'0000});
        XSTD_CONSTEXPR_CHECK(xstd::bit_reverse(std::uint32_t{0x2c48'0000}) == std::uint32_t{0x0000'1234});
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

// Unconditional where the operations cannot throw, and otherwise as conditional as the type's own operators.
BOOST_AUTO_TEST_CASE_TEMPLATE(NoexceptFollowsTheOperators, T, permutable_types)
{
        static_assert(noexcept(xstd::bit_reverse(T{1})) == xstd::nothrow_const_operators<T>);
        BOOST_CHECK(true);
}

// The lowest bit becomes the highest, and the bit past a 64-bit word lands that far below the top.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsTrade, T, permutable_types)
{
        constexpr auto N   = xstd::numeric_limits<T>::digits;
        constexpr auto top = static_cast<T>(T{1} << (N - 1));
        static_assert(xstd::bit_reverse(T{0}) == T{0});
        static_assert(xstd::bit_reverse(T{1}) == top);
        static_assert(xstd::bit_reverse(top) == T{1});
        static_assert(xstd::bit_reverse(xstd::numeric_limits<T>::max()) == xstd::numeric_limits<T>::max());
        if constexpr (N > 64) {
                static_assert(xstd::bit_reverse(static_cast<T>(T{1} << 64)) == static_cast<T>(T{1} << (N - 65)));
        }
        BOOST_CHECK(true);
}

// [bit.permute]'s note: reversing twice is the identity.
BOOST_AUTO_TEST_CASE_TEMPLATE(ReversingTwiceIsTheIdentity, T, permutable_types)
{
        auto mismatches = 0UZ;
        test::for_each_sweep_value<T>([&](T x) -> void { mismatches += xstd::bit_reverse(xstd::bit_reverse(x)) == x ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// Each width against a bit-at-a-time reference: its edges at compile time, every value or a sweep at run time.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheReference, T, permutable_types)
{
        static_assert(test::holds_at_every_position<agrees_at<T>>(std::make_integer_sequence<int, xstd::numeric_limits<T>::digits>{}));
        auto mismatches = 0UZ;
        test::for_each_sweep_value<T>([&](T x) -> void { mismatches += xstd::bit_reverse(x) == test::reference_bit_reverse(x) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
