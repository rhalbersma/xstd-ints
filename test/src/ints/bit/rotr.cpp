//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/rotr.hpp>                         // rotr
#include <xstd/ints/bit/rotl.hpp>                         // rotl
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdint.hpp>                          // bit_int, bit_uint, int128, uint128
#include <xstd/ints/limits.hpp>                           // numeric_limits
#include <test/bit_reference.hpp>                         // bit_precise_sweep_types, for_each_sweep_value, reference_rotl
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>                     // absl_unsigned_types, boost_unsigned_types, std_unsigned_types, xstd_unsigned_types
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                                          // array
#include <bit>                                            // rotr
#include <cstdint>                                        // int32_t, uint64_t, uint8_t
#include <limits>                                         // numeric_limits
#include <tuple>                                          // tuple_cat
#include <utility>                                        // declval

namespace {

using basis_unsigned_types = decltype(std::tuple_cat(
        std::declval<test::std_unsigned_types>(), std::declval<test::xstd_unsigned_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_unsigned_types>(),
        std::declval<test::bit_precise_sweep_types>()
));

template<class T>
concept xstd_answers = requires (T x, int s) { xstd::rotr(x, s); };

template<class T>
concept std_answers = requires (T x, int s) { std::rotr(x, s); };

// Counts that reduce differently: zero, one and the width with its neighbours either way, and the int extremes.
template<class T>
constexpr auto counts() noexcept
{
        constexpr auto N = xstd::numeric_limits<T>::digits;
        return std::array{0, 1, -1, N - 1, N, N + 1, 1 - N, -N, -N - 1, (2 * N) + 3, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(Rotr)

// Where <bit> answers, this IS <bit>: the overload forwards and must not drift from it.
BOOST_AUTO_TEST_CASE(AgreesWithStdWhereStdAnswers)
{
        XSTD_CONSTEXPR_CHECK(xstd::rotr(std::uint8_t{0x81}, 1) == std::rotr(std::uint8_t{0x81}, 1));
        XSTD_CONSTEXPR_CHECK(xstd::rotr(std::uint8_t{0x81}, -9) == std::rotr(std::uint8_t{0x81}, -9));
        XSTD_CONSTEXPR_CHECK(xstd::rotr(std::uint64_t{0x1234}, 65) == std::rotr(std::uint64_t{0x1234}, 65));
        XSTD_CONSTEXPR_CHECK(xstd::rotr(std::uint64_t{0x1234}, -1) == std::rotr(std::uint64_t{0x1234}, -1));
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
        static_assert(xstd_answers<xstd::uint128>);
        static_assert(not xstd_answers<xstd::int128>);
#ifdef XSTD_HAS_BIT_INT
        static_assert(xstd_answers<xstd::bit_uint<24>>);
        static_assert(not xstd_answers<xstd::bit_int<24>>);
#endif
        BOOST_CHECK(true);
}

// Unconditional where the operations cannot throw, and otherwise as conditional as the type's own operators.
BOOST_AUTO_TEST_CASE_TEMPLATE(NoexceptFollowsTheOperators, T, basis_unsigned_types)
{
        static_assert(noexcept(xstd::rotr(T{1}, 1)) == xstd::nothrow_const_operators<T>);
        BOOST_CHECK(true);
}

// The bit at either end crosses to the other, and a whole turn in either direction is no turn at all.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsTrade, T, basis_unsigned_types)
{
        constexpr auto N    = xstd::numeric_limits<T>::digits;
        constexpr auto ones = xstd::numeric_limits<T>::max();
        constexpr auto top  = static_cast<T>(ones ^ static_cast<T>(ones >> 1U));
        static_assert(xstd::rotr(T{1}, 1) == top);
        static_assert(xstd::rotr(top, -1) == T{1});
        static_assert(xstd::rotr(T{1}, N) == T{1});
        static_assert(xstd::rotr(T{1}, -N) == T{1});
        static_assert(xstd::rotr(ones, 1) == ones);
        BOOST_CHECK(true);
}

// Turning one way and then the other by the same count is the identity, the int extremes included.
BOOST_AUTO_TEST_CASE_TEMPLATE(RotrUndoesRotl, T, basis_unsigned_types)
{
        auto mismatches = 0UZ;
        test::for_each_sweep_value<T>([&](T x) -> void {
                for (auto const s : counts<T>()) {
                        mismatches += xstd::rotl(xstd::rotr(x, s), s) == x ? 0UZ : 1UZ;
                }
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// Each width against a reference turning one place at a time, at every count that reduces differently.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheReference, T, basis_unsigned_types)
{
        auto mismatches = 0UZ;
        test::for_each_sweep_value<T>([&](T x) -> void {
                for (auto const s : counts<T>()) {
                        mismatches += xstd::rotr(x, s) == test::reference_rotl(x, -(s % xstd::numeric_limits<T>::digits)) ? 0UZ : 1UZ;
                }
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
