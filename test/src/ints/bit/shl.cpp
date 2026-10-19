//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/detail/shift.hpp>                 // IWYU pragma: keep; shift, shift_direction, where the standard library ships std::shl
#include <xstd/ints/bit/shl.hpp>                          // shl
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdint.hpp>                          // XSTD_HAS_BIT_INT, XSTD_HAS_INT128, bit_int, bit_uint, int128, uint128
#include <xstd/ints/limits.hpp>                           // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp>            // is_signed_v
#include <xstd/ints/type_traits/make_unsigned.hpp>        // make_unsigned_t
#include <test/bit/oracle.hpp>                            // oracle_shift
#include <test/bit/sweep.hpp>                             // for_each_edge_value, for_each_sweep_value
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK
#include <test/exact_width_types.hpp>                     // constexpr_exact_width_signed_integer_types, constexpr_exact_width_unsigned_integer_types, exact_width_integer_types
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                                          // array
#include <bit>                                            // IWYU pragma: keep; shl, where the standard library ships it
#include <cstdint>                                        // int64_t, int8_t, uint32_t, uint64_t, uint8_t
#include <limits>                                         // numeric_limits
#include <tuple>                                          // apply, tuple, tuple_cat
#include <utility>                                        // declval
#include <version>                                        // IWYU pragma: keep; __cpp_lib_bitops

namespace {

template<class T, class S>
concept xstd_answers = requires (T x, S s) { xstd::shl(x, s); };

// Counts that behave differently: zero, one and the width with its neighbours either way, and the int extremes.
template<class T>
constexpr auto counts() noexcept
{
        constexpr auto W = xstd::numeric_limits<xstd::make_unsigned_t<T>>::digits;
        return std::array{0, 1, -1, W - 1, W, W + 1, 1 - W, -W, -W - 1, 2 * W, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
}

// A count type holds c when c survives the trip into it and back, and a negative c needs a signed type.
template<class S>
constexpr auto holds(int c) noexcept
        -> bool
{
        return (c >= 0 or xstd::is_signed_v<S>) and static_cast<int>(static_cast<S>(c)) == c;
}

// The count types: the narrowest either way, int, the widest standard ones either way, and the 128-bit pair if any.
#ifdef XSTD_HAS_INT128
using count_types = std::tuple<signed char, unsigned char, int, std::int64_t, std::uint64_t, xstd::int128, xstd::uint128>;
#else
using count_types = std::tuple<signed char, unsigned char, int, std::int64_t, std::uint64_t>;
#endif

// Every exact width whose operators constant-evaluate, which leaves out only Abseil's fallback.
using constexpr_integer_types = decltype(std::tuple_cat(std::declval<test::constexpr_exact_width_signed_integer_types>(), std::declval<test::constexpr_exact_width_unsigned_integer_types>()));

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Bit)
BOOST_AUTO_TEST_SUITE(Shl)

// P3793's rule: the exact product rounded towards negative infinity, so no count is out of range and none is undefined.
BOOST_AUTO_TEST_CASE(EveryCountIsDefined)
{
        XSTD_CONSTEXPR_CHECK(xstd::shl(1U, 32) == 0U);
        XSTD_CONSTEXPR_CHECK(xstd::shl(8, -2) == 2);
        XSTD_CONSTEXPR_CHECK(xstd::shl(-5, -1) == -3);
        XSTD_CONSTEXPR_CHECK(xstd::shl(std::int8_t{-1}, 7) == std::int8_t{-128});
        XSTD_CONSTEXPR_CHECK(xstd::shl(std::int8_t{-1}, 8) == std::int8_t{0});
        XSTD_CONSTEXPR_CHECK(xstd::shl(1, std::numeric_limits<int>::min()) == 0);
        XSTD_CONSTEXPR_CHECK(xstd::shl(-1, std::numeric_limits<int>::min()) == -1);
        XSTD_CONSTEXPR_CHECK(xstd::shl(std::uint8_t{3}, std::uint64_t{1} << 63U) == std::uint8_t{0});
}

#if defined(__cpp_lib_bitops) and __cpp_lib_bitops >= 202606L

// The template every other type takes, called where <bit> answers too, so that the standard library is its oracle.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheFallbackAgreesWithStd, T, test::exact_width_integer_types)
{
        if constexpr (requires (T x, int s) { std::shl(x, s); }) {
                using U         = xstd::make_unsigned_t<T>;
                auto mismatches = 0UZ;
                test::for_each_sweep_value<U>([&](U u) -> void {
                        auto const& x = static_cast<T>(u);
                        for (auto const c : counts<T>()) {
                                mismatches += xstd::ints::detail::shift(x, c, xstd::ints::detail::shift_direction::left) == std::shl(x, c) ? 0UZ : 1UZ;
                        }
                });
                BOOST_CHECK_EQUAL(mismatches, 0UZ);
        } else {
                BOOST_CHECK(true);
        }
}

// Where <bit> answers, this IS <bit>: the overload forwards and must not drift from it.
BOOST_AUTO_TEST_CASE(AgreesWithStdWhereStdAnswers)
{
        XSTD_CONSTEXPR_CHECK(xstd::shl(-5, 1) == std::shl(-5, 1));
        XSTD_CONSTEXPR_CHECK(xstd::shl(1U, 40) == std::shl(1U, 40));
        XSTD_CONSTEXPR_CHECK(xstd::shl(std::uint64_t{7}, -3) == std::shl(std::uint64_t{7}, -3));
}

#endif

// Both operands range over every integer xstd knows, signed or unsigned, and over nothing that only looks like one.
BOOST_AUTO_TEST_CASE(TheConstraintIsInteger)
{
        static_assert(xstd_answers<int, int>);
        static_assert(xstd_answers<std::uint8_t, std::int64_t>);
#ifdef XSTD_HAS_INT128
        static_assert(xstd_answers<xstd::int128, xstd::uint128>);
        static_assert(xstd_answers<std::uint32_t, xstd::int128>);
#endif
        static_assert(not xstd_answers<bool, int>);
        static_assert(not xstd_answers<int, bool>);
        static_assert(not xstd_answers<char8_t, int>);
        static_assert(not xstd_answers<int, char>);
#ifdef XSTD_HAS_BIT_INT
        static_assert(xstd_answers<xstd::bit_int<24>, xstd::bit_uint<13>>);
#endif
        BOOST_CHECK(true);
}

// Unconditional where the operations cannot throw, and otherwise as conditional as the type's own operators.
BOOST_AUTO_TEST_CASE_TEMPLATE(NoexceptFollowsTheOperators, T, test::exact_width_integer_types)
{
        static_assert(noexcept(xstd::shl(T{1}, 1)) == xstd::nothrow_const_operators<T>);
        static_assert(noexcept(xstd::shl(1, T{1})) == xstd::nothrow_const_operators<T>);
        BOOST_CHECK(true);
}

// At every width and at compile time: up to the top bit and past it, down past the bottom, and a count of type T.
BOOST_AUTO_TEST_CASE_TEMPLATE(TheEndsOfEveryWidth, T, constexpr_integer_types)
{
        using U                    = xstd::make_unsigned_t<T>;
        constexpr auto W           = xstd::numeric_limits<U>::digits;
        constexpr auto ones        = static_cast<T>(xstd::numeric_limits<U>::max());
        constexpr auto top         = static_cast<T>(U{1} << static_cast<unsigned>(W - 1));
        constexpr auto shifted_out = xstd::is_signed_v<T> ? ones : T{0};
        static_assert(xstd::shl(T{1}, W - 1) == top);
        static_assert(xstd::shl(T{1}, W) == T{0});
        static_assert(xstd::shl(T{2}, -1) == T{1});
        static_assert(xstd::shl(T{1}, -1) == T{0});
        static_assert(xstd::shl(ones, -W) == shifted_out);
        static_assert(xstd::shl(ones, std::numeric_limits<int>::min()) == shifted_out);
        static_assert(xstd::shl(T{1}, static_cast<T>(W)) == T{0});
        static_assert(xstd::shl(T{1}, top) == T{0});
        BOOST_CHECK(true);
}

// Each width against an oracle moving one place at a time, at every count that behaves differently.
BOOST_AUTO_TEST_CASE_TEMPLATE(AgreesWithTheOracle, T, test::exact_width_integer_types)
{
        using U         = xstd::make_unsigned_t<T>;
        auto mismatches = 0UZ;
        test::for_each_sweep_value<U>([&](U u) -> void {
                // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const& x = static_cast<T>(u);
                for (auto const c : counts<T>()) {
                        mismatches += xstd::shl(x, c) == test::oracle_shift(x, c) ? 0UZ : 1UZ;
                }
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

// The count's type changes nothing but which counts it can hold.
BOOST_AUTO_TEST_CASE_TEMPLATE(EveryCountTypeAgrees, T, test::exact_width_integer_types)
{
        using U         = xstd::make_unsigned_t<T>;
        auto mismatches = 0UZ;
        test::for_each_edge_value<U>([&](U u) -> void {
                // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const& x = static_cast<T>(u);
                for (auto const c : counts<T>()) {
                        std::apply([&]<class... S>(S...) -> void { ((mismatches += not holds<S>(c) or xstd::shl(x, static_cast<S>(c)) == xstd::shl(x, c) ? 0UZ : 1UZ), ...); }, count_types{});
                }
        });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
