//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_repeat.hpp>            // bit_repeat
#include <xstd/ints/bit/bit_reverse.hpp>           // bit_reverse
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp> // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/bit/popcount.hpp>              // popcount
#include <fuzz/input.hpp>                          // byte_width, check, read_at, unsigned_types
#include <test/bit/oracle.hpp>                     // oracle_bit_repeat, oracle_bit_reverse
#include <test/bit/permutation_properties.hpp>     // intrinsics_agree, permutations_hold, portable_kernels_round_trip
#include <test/bit/select_properties.hpp>          // intrinsic_select_agrees, portable_select_holds, selection_holds
#include <cstddef>                                 // size_t
#include <cstdint>                                 // uint8_t
#include <span>                                    // span
#include <tuple>                                   // apply

namespace {

// The source's lowest and highest ones, and the one whose rank is the mask's population reduced modulo the source's.
template<class T>
[[nodiscard]] auto selections_hold(T src, T mask)
        -> bool
{
        auto const ones = xstd::popcount(src);
        return ones == 0 or (test::selection_holds(src, 0) and test::selection_holds(src, xstd::popcount(mask) % ones) and test::selection_holds(src, ones - 1));
}

// The source reversed, and repeated with the period one more than the mask's population, past the width included.
template<class T>
[[nodiscard]] auto reversal_and_repetition_hold(T src, T mask)
        -> bool
{
        auto const period = xstd::popcount(mask) + 1;
        return xstd::bit_reverse(src) == test::oracle_bit_reverse(src) and xstd::bit_repeat(src, period) == test::oracle_bit_repeat(src, period);
}

// The source from the input's first bytes and the mask from those after it, so every width reads the same prefix.
template<class T>
[[nodiscard]] auto holds_at(std::span<std::uint8_t const> input)
        -> bool
{
        // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
        auto const& src  = fuzz::read_at<T>(input, 0UZ);
        auto const& mask = fuzz::read_at<T>(input, fuzz::byte_width<T>);
        return test::permutations_hold(src, mask) and selections_hold(src, mask) and reversal_and_repetition_hold(src, mask);
}

} // namespace

extern "C" auto LLVMFuzzerTestOneInput(std::uint8_t const* data, std::size_t size)
        -> int
{
        auto const input = std::span(data, size);
        auto const src   = fuzz::read_at<std::size_t>(input, 0UZ);
        auto const mask  = fuzz::read_at<std::size_t>(input, sizeof(std::size_t));
        fuzz::check(test::portable_kernels_round_trip(src, mask));
        fuzz::check(test::portable_select_holds(src));
#ifdef XSTD_INTS_HAS_FAST_BMI2
        fuzz::check(test::intrinsics_agree(src, mask));
        fuzz::check(test::intrinsic_select_agrees(src));
#endif
        std::apply([&]<class... T>(T...) -> void { (fuzz::check(holds_at<T>(input)), ...); }, fuzz::unsigned_types{});
        return 0;
}
