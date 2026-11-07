//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div.hpp>           // div
#include <xstd/ints/limits.hpp>                // numeric_limits
#include <test/bit/sweep.hpp>                  // next_pseudo_random
#include <test/boost_test_print_log_value.hpp> // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>          // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>            // Boost.Test
#include <array>                               // array
#include <cinttypes>                           // imaxdiv, imaxdiv_t
#include <cstdint>                             // intmax_t, uint64_t
#include <cstdlib>                             // div, div_t, ldiv, ldiv_t, lldiv, lldiv_t
#include <ranges>                              // iota

namespace {

// The edge values crossed with each other, then a reproducible sample of random pairs at full and at small divisors.
template<class T, class StdDiv>
auto check_agrees_with(StdDiv std_div)
        -> void
{
        using limits     = xstd::numeric_limits<T>;
        auto const check = [&](T numer, T denom) -> void {
                // Both leave a zero divisor and the one quotient past max() undefined, as [expr.mul]/4 does.
                if (denom == T{0} or (numer == limits::min() and denom == T{-1})) {
                        return;
                }
                auto const expected = std_div(numer, denom);
                BOOST_CHECK_EQUAL(xstd::div(numer, denom), (xstd::div_result<T>{expected.quot, expected.rem}));
        };
        auto const edges = std::array{limits::min(), static_cast<T>(limits::min() + 1), T{-2}, T{-1}, T{0}, T{1}, T{2}, static_cast<T>(limits::max() - 1), limits::max()};
        for (auto const numer : edges) {
                for (auto const denom : edges) {
                        check(numer, denom);
                }
        }
        auto state = std::uint64_t{0x2545'f491'4f6c'dd1dU};
        for ([[maybe_unused]] auto const sample : std::views::iota(0UZ, 1UZ << 16U)) {
                auto const numer = test::next_pseudo_random<T>(state);
                auto const denom = test::next_pseudo_random<T>(state);
                check(numer, denom);
                check(numer, static_cast<T>(denom % T{1000}));
        }
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)
BOOST_AUTO_TEST_SUITE(Div)

// http://research.microsoft.com/pubs/151917/divmodnote-letter.pdf

BOOST_AUTO_TEST_CASE_TEMPLATE(TruncatesTowardZero, T, test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{+8}, T{+3})), (xstd::div_result<T>{+2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{+8}, T{-3})), (xstd::div_result<T>{-2, +2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-8}, T{+3})), (xstd::div_result<T>{-2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-8}, T{-3})), (xstd::div_result<T>{+2, -2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{+1}, T{+2})), (xstd::div_result<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{+1}, T{-2})), (xstd::div_result<T>{0, +1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-1}, T{+2})), (xstd::div_result<T>{0, -1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-1}, T{-2})), (xstd::div_result<T>{0, -1}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TruncatesTowardZeroOnUnsigned, T, test::exact_width_unsigned_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{8}, T{3})), (xstd::div_result<T>{2, 2}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{1}, T{2})), (xstd::div_result<T>{0, 1}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{0}, T{3})), (xstd::div_result<T>{0, 0}));

        // min() and static_cast<T>(-1) together, which unsigned is just 0 and max().
        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(limits::min(), limits::max())), (xstd::div_result<T>{0, 0}));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(limits::max(), limits::max())), (xstd::div_result<T>{1, 0}));
}

// [cstdlib.syn] names one function per type, with its own result type and the member names quot and rem.
BOOST_AUTO_TEST_CASE(AgreesWithTheStandardFunctions)
{
        check_agrees_with<int>([](int numer, int denom) -> std::div_t { return std::div(numer, denom); });
        check_agrees_with<long>([](long numer, long denom) -> std::ldiv_t { return std::ldiv(numer, denom); });
        check_agrees_with<long long>([](long long numer, long long denom) -> std::lldiv_t { return std::lldiv(numer, denom); });
        check_agrees_with<std::intmax_t>([](std::intmax_t numer, std::intmax_t denom) -> std::imaxdiv_t { return std::imaxdiv(numer, denom); });
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
