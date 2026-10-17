//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp> // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <test/bit/permutation_properties.hpp>     // intrinsics_agree, permutations_hold, portable_kernels_round_trip
#include <sweep.hpp>                               // for_each_row, passed, patterns
#include <atomic>                                  // atomic
#include <cstddef>                                 // size_t
#include <cstdint>                                 // uint16_t, uint64_t

// Every 16-bit source against every 16-bit mask: the public functions, the portable kernels, and PEXT and PDEP.
auto main() -> int
{
        auto failures = std::atomic<std::uint64_t>{0U};
        exhaustive::for_each_row([&](std::uint16_t src) -> void {
                auto local = std::uint64_t{0U};
                for (auto m = 0U; m < exhaustive::patterns; ++m) {
                        auto const mask  = static_cast<std::uint16_t>(m);
                        auto const block = std::size_t{mask};
                        auto holds       = test::permutations_hold(src, mask) and test::portable_kernels_round_trip(std::size_t{src}, block);
#ifdef XSTD_INTS_HAS_FAST_BMI2
                        holds = holds and test::intrinsics_agree(std::size_t{src}, block);
#endif
                        local += holds ? 0U : 1U;
                }
                failures.fetch_add(local, std::memory_order_relaxed);
        });
        return exhaustive::passed("bit_compress and bit_expand over every 16-bit pair", failures.load()) ? 0 : 1;
}
