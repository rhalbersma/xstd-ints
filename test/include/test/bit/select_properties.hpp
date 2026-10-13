//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_BIT_SELECT_PROPERTIES_HPP
#define TEST_BIT_SELECT_PROPERTIES_HPP

#include <xstd/ints/bit/bit_expand.hpp>                    // bit_expand
#include <xstd/ints/bit/bit_select.hpp>                    // bit_select
#include <xstd/ints/bit/countr_zero.hpp>                   // countr_zero
#include <xstd/ints/bit/detail/intrinsic/block_select.hpp> // IWYU pragma: keep; block_select, where PDEP is fast
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp>         // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/bit/detail/portable/block_select.hpp>  // block_select_by_broadword
#include <xstd/ints/bit/popcount.hpp>                      // popcount
#include <test/bit/oracle.hpp>                             // oracle_bit_select
#include <test/bit/shift_count.hpp>                        // shift_count_t
#include <bit>                                             // popcount
#include <cstddef>                                         // size_t
#include <ranges>                                          // iota

// What selection owes rank, expansion and its oracle, shared by the unit tests and the fuzzer.
namespace test {

// Every rank of one block: the portable kernel finds the one the oracle does.
[[nodiscard]] constexpr auto portable_select_holds(std::size_t x)
        -> bool
{
        auto holds = true;
        for (auto const k : std::views::iota(0, std::popcount(x))) {
                holds = holds and xstd::ints::detail::portable::block_select_by_broadword(x, static_cast<unsigned>(k)) == static_cast<unsigned>(oracle_bit_select(x, k));
        }
        return holds;
}

#ifdef XSTD_INTS_HAS_FAST_BMI2

// Every rank of one block: the instruction finds the one the portable kernel does.
[[nodiscard]] inline auto intrinsic_select_agrees(std::size_t x)
        -> bool
{
        auto agrees = true;
        for (auto const k : std::views::iota(0U, static_cast<unsigned>(std::popcount(x)))) {
                agrees = agrees and xstd::ints::detail::intrinsic::block_select(x, k) == xstd::ints::detail::portable::block_select_by_broadword(x, k);
        }
        return agrees;
}

#endif

// The k-th one of x has exactly k ones below it, and is where expanding a lone k-th bit through x deposits it.
template<class T>
[[nodiscard]] constexpr auto selection_inverts_rank(T x, int k)
        -> bool
{
        using shift_count = shift_count_t<T>;
        auto const at     = static_cast<shift_count>(xstd::bit_select(x, k));
        // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
        auto const& found = static_cast<T>(T{1} << at);
        auto const& below = static_cast<T>(found - T{1});
        auto const& lone  = static_cast<T>(T{1} << static_cast<shift_count>(k));
        return static_cast<T>(x & found) != T{0} and xstd::popcount(static_cast<T>(x & below)) == k and
               xstd::countr_zero(xstd::bit_expand(lone, x)) == static_cast<int>(at);
}

// At rank k the public function agrees with its oracle and inverts rank.
template<class T>
[[nodiscard]] constexpr auto selection_holds(T x, int k)
        -> bool
{
        return xstd::bit_select(x, k) == oracle_bit_select(x, k) and selection_inverts_rank(x, k);
}

} // namespace test

#endif // TEST_BIT_SELECT_PROPERTIES_HPP
