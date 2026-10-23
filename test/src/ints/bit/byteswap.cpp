//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/byteswap.hpp>                     // byteswap
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdint.hpp>                          // XSTD_HAS_BIT_INT, XSTD_HAS_INT128, bit_int, bit_uint, int128, uint128
#include <xstd/ints/limits.hpp>                           // numeric_limits
#include <xstd/ints/type_traits/make_unsigned.hpp>        // make_unsigned_t
#include <test/bit/oracle.hpp>                            // oracle_byteswap
#include <test/bit/sweep.hpp>                             // for_each_edge_value_at, for_each_sweep_value, holds_at_every_position
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>                     // exact_width_integer_types
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <bit>                                            // byteswap
#include <cstdint>                                        // int16_t, int32_t, uint16_t, uint32_t, uint64_t
#include <tuple>                                          // tuple, tuple_cat
#include <utility>                                        // declval, make_integer_sequence

namespace {

// Three bytes in a four-byte object, a width whose padding bits the standard function's Mandates turn away.
#ifdef XSTD_HAS_BIT_INT
using padded_whole_byte_types = std::tuple<xstd::bit_int<24>, xstd::bit_uint<24>>;
#else
using padded_whole_byte_types = std::tuple<>;
#endif

using swappable_types = decltype(std::tuple_cat(std::declval<test::exact_width_integer_types>(), std::declval<padded_whole_byte_types>()));

template<class T>
concept xstd_answers = requires (T x) { xstd::byteswap(x); };

template<class T>
concept std_answers = requires (T x) { std::byteswap(x); };

// The edge values at one position of the unsigned counterpart, read as T, so that no signed doubling overflows.
template<class T>
[[nodiscard]] constexpr auto agrees_at(int k)
        -> bool
{
        auto agrees = true;
        test::for_each_edge_value_at<xstd::make_unsigned_t<T>>(k, [&](xstd::make_unsigned_t<T> u) -> void {
                // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const& x = static_cast<T>(u);
                agrees        = agrees and xstd::byteswap(x) == test::oracle_byteswap(x);
        });
        return agrees;
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(Byteswap)

// Where <bit> answers, this IS <bit>: the overload forwards and must not drift from it.
BOOST_AUTO_TEST_CASE(AgreesWithStdWhereStdAnswers)
{
        XSTD_CONSTEXPR_CHECK(xstd::byteswap(std::uint16_t{0x1234}) == std::byteswap(std::uint16_t{0x1234}));
        XSTD_CONSTEXPR_CHECK(xstd::byteswap(std::int16_t{-2}) == std::byteswap(std::int16_t{-2}));
        XSTD_CONSTEXPR_CHECK(xstd::byteswap(std::uint32_t{0x1234'5678}) == std::byteswap(std::uint32_t{0x1234'5678}));
        XSTD_CONSTEXPR_CHECK(xstd::byteswap(std::uint64_t{0x0102'0304'0506'0708}) == std::byteswap(std::uint64_t{0x0102'0304'0506'0708}));
}

// <bit>'s own answer wherever it gives one, widened to every integer xstd knows whose width is a whole number of bytes.
BOOST_AUTO_TEST_CASE(TheConstraintIsAWholeNumberOfBytes)
{
        static_assert(xstd_answers<bool> == std_answers<bool>);
        static_assert(xstd_answers<char8_t> == std_answers<char8_t>);
        static_assert(xstd_answers<std::int32_t> == std_answers<std::int32_t>);
        static_assert(xstd_answers<std::uint64_t> == std_answers<std::uint64_t>);
#ifdef XSTD_HAS_INT128
        static_assert(xstd_answers<xstd::int128>);
        static_assert(xstd_answers<xstd::uint128>);
#endif
#ifdef XSTD_HAS_BIT_INT
        // A width of 24 bits fills no whole object, which <bit> rejects; xstd swaps its three bytes.
        static_assert(xstd_answers<xstd::bit_int<24>>);
        static_assert(xstd_answers<xstd::bit_uint<24>>);
        static_assert(not xstd_answers<xstd::bit_int<13>>);
        static_assert(not xstd_answers<xstd::bit_uint<13>>);
#endif
        BOOST_CHECK(true);
}

// Unconditional where the operations cannot throw, and otherwise as conditional as the operators of the pair.
BOOST_AUTO_TEST_CASE_TEMPLATE(NoexceptFollowsTheOperators, T, swappable_types)
{
        static_assert(noexcept(xstd::byteswap(T{1})) == (xstd::nothrow_const_operators<T> and xstd::nothrow_const_operators<xstd::make_unsigned_t<T>>));
        BOOST_CHECK(true);
}

// The lowest byte becomes the highest, and the highest the lowest.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsTrade, T, swappable_types)
{
        using U            = xstd::make_unsigned_t<T>;
        constexpr auto top = static_cast<U>(U{0xA5} << static_cast<unsigned>(xstd::numeric_limits<U>::digits - 8));
        static_assert(xstd::byteswap(T{0}) == T{0});
        static_assert(xstd::byteswap(static_cast<T>(U{0xA5})) == static_cast<T>(top));
        static_assert(xstd::byteswap(static_cast<T>(top)) == static_cast<T>(U{0xA5}));
        static_assert(xstd::byteswap(static_cast<T>(xstd::numeric_limits<U>::max())) == static_cast<T>(xstd::numeric_limits<U>::max()));
        BOOST_CHECK(true);
}

// Each width against a byte-at-a-time oracle, and swapping twice is the identity.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheOracle, T, swappable_types)
{
        using U = xstd::make_unsigned_t<T>;
        static_assert(test::holds_at_every_position<agrees_at<T>>(std::make_integer_sequence<int, xstd::numeric_limits<U>::digits>{}));
        auto mismatches = 0UZ;
        test::for_each_sweep_value<U>([&](U u) -> void {
                // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const& x = static_cast<T>(u);
                mismatches += xstd::byteswap(x) == test::oracle_byteswap(x) ? 0UZ : 1UZ;
                mismatches += xstd::byteswap(xstd::byteswap(x)) == x ? 0UZ : 1UZ;
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
