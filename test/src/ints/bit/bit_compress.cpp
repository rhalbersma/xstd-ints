//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_compress.hpp>                 // bit_compress
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdint.hpp>                          // bit_int, bit_uint, uint128
#include <xstd/ints/limits.hpp>                           // numeric_limits
#include <test/bit_reference.hpp>                         // alternating_ones, bit_precise_sweep_types, for_each_edge_value_at, for_each_sweep_pair, for_each_sweep_value, holds_at_every_position, reference_bit_compress, width
#include <test/exact_width_types.hpp>                     // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                                          // array
#include <cstdint>                                        // int32_t, uint64_t
#include <ranges>                                         // iota
#include <tuple>                                          // tuple_cat
#include <utility>                                        // declval, make_integer_sequence

namespace {

using permutable_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>(),
        std::declval<test::bit_precise_sweep_types>()
));

template<class T>
concept xstd_answers = requires (T x) { xstd::bit_compress(x, x); };

template<class T>
constexpr auto agrees_at(int k)
        -> bool
{
        auto agrees = true;
        // An array, not a braced list: MSVC 19.44 crashes constant-evaluating a loop over the latter.
        for (auto const x : std::array{xstd::numeric_limits<T>::max(), test::alternating_ones<T>()}) {
                test::for_each_edge_value_at<T>(k, [&](T m) -> void {
                        agrees = agrees and xstd::bit_compress(x, m) == test::reference_bit_compress(x, m);
                        agrees = agrees and xstd::bit_compress(m, x) == test::reference_bit_compress(m, x);
                });
        }
        return agrees;
}

// [bit.permute]'s example: bit_compress(0bABCD, 0b0101) is 0b00BD for any bits A, B, C and D.
template<class T>
constexpr auto compresses_as_the_example()
        -> bool
{
        auto agrees = true;
        for (auto const abcd : std::views::iota(0U, 16U)) {
                auto const x = static_cast<T>(abcd);
                auto const b = static_cast<T>(static_cast<T>(x >> 2) & T{1});
                auto const d = static_cast<T>(x & T{1});
                agrees       = agrees and xstd::bit_compress(x, static_cast<T>(0b0101U)) == static_cast<T>(static_cast<T>(b << 1) | d);
        }
        return agrees;
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(BitCompress)

BOOST_AUTO_TEST_CASE_TEMPLATE(TheWordingsExample, T, permutable_types)
{
        if constexpr (test::width<T> >= 4UZ) {
                static_assert(compresses_as_the_example<T>());
        }
        BOOST_CHECK(true);
}

// P3104R5's equivalences: a contiguous mask is a shift and an and, and a single bit reads that bit.
BOOST_AUTO_TEST_CASE_TEMPLATE(ThePapersEquivalences, T, permutable_types)
{
        if constexpr (test::width<T> >= 8UZ) {
                auto mismatches = 0UZ;
                test::for_each_sweep_value<T>([&](T x) -> void {
                        mismatches += xstd::bit_compress(x, static_cast<T>(0xfU)) == static_cast<T>(x & static_cast<T>(0xfU)) ? 0UZ : 1UZ;
                        mismatches += xstd::bit_compress(x, static_cast<T>(0xf0U)) == static_cast<T>(static_cast<T>(x >> 4) & static_cast<T>(0xfU)) ? 0UZ : 1UZ;
                        for (auto const n : std::views::iota(0, xstd::numeric_limits<T>::digits)) {
                                mismatches += xstd::bit_compress(x, static_cast<T>(T{1} << n)) == static_cast<T>(static_cast<T>(x >> n) & T{1}) ? 0UZ : 1UZ;
                        }
                });
                BOOST_CHECK_EQUAL(mismatches, 0UZ);
        } else {
                BOOST_CHECK(true);
        }
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

// As conditional as the type's own operators, which makes it unconditional for every built-in type.
BOOST_AUTO_TEST_CASE_TEMPLATE(NoexceptFollowsTheOperators, T, permutable_types)
{
        static_assert(noexcept(xstd::bit_compress(T{1}, T{1})) == xstd::nothrow_const_operators<T>);
        BOOST_CHECK(true);
}

// An empty mask selects nothing, a full one everything, and only the top bit moves the top bit to the bottom.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheExtremeMasks, T, permutable_types)
{
        constexpr auto ones = xstd::numeric_limits<T>::max();
        constexpr auto top  = static_cast<T>(T{1} << (xstd::numeric_limits<T>::digits - 1));
        static_assert(xstd::bit_compress(ones, T{0}) == T{0});
        static_assert(xstd::bit_compress(ones, ones) == ones);
        static_assert(xstd::bit_compress(top, top) == T{1});
        if constexpr (test::width<T> > 64UZ) {
                constexpr auto straddle = static_cast<T>(static_cast<T>(T{1} << 63) | static_cast<T>(T{1} << 64));
                static_assert(xstd::bit_compress(straddle, straddle) == T{3});
                static_assert(xstd::bit_compress(static_cast<T>(T{1} << 64), straddle) == T{2});
        }
        BOOST_CHECK(true);
}

// Each width against a bit-at-a-time reference: its edges at compile time, every pair or a sweep at run time.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheReference, T, permutable_types)
{
        static_assert(test::holds_at_every_position<agrees_at<T>>(std::make_integer_sequence<int, xstd::numeric_limits<T>::digits>{}));
        auto mismatches = 0UZ;
        test::for_each_sweep_pair<T>([&](T x, T m) -> void { mismatches += xstd::bit_compress(x, m) == test::reference_bit_compress(x, m) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
