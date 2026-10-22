//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef EXHAUSTIVE_SWEEP_HPP
#define EXHAUSTIVE_SWEEP_HPP

#include <algorithm> // max, min
#include <cstdint>   // uint16_t, uint64_t
#include <iostream>  // cerr
#include <thread>    // jthread
#include <vector>    // vector

namespace exhaustive {

inline constexpr auto patterns = 0x1'0000U;

// Each of the 2^16 rows once, the rows split across threads; every row is one first operand against all second ones.
template<class Row>
auto for_each_row(Row row)
        -> void
{
        auto const threads = std::max(1U, std::thread::hardware_concurrency());
        auto const share   = (patterns + threads - 1U) / threads;
        auto workers       = std::vector<std::jthread>{};
        for (auto t = 0U; t < threads; ++t) {
                auto const first = std::min(t * share, patterns);
                auto const last  = std::min(first + share, patterns);
                workers.emplace_back([=]() -> void {
                        for (auto a = first; a < last; ++a) {
                                row(static_cast<std::uint16_t>(a));
                        }
                });
        }
}

// SplitMix64's finalizer: a sum of it over a set of words changes with any one word, and not with their order.
[[nodiscard]] constexpr auto mix(std::uint64_t x) noexcept
        -> std::uint64_t
{
        x += 0x9E37'79B9'7F4A'7C15ULL;
        x = (x ^ (x >> 30U)) * 0xBF58'476D'1CE4'E5B9ULL;
        x = (x ^ (x >> 27U)) * 0x94D0'49BB'1331'11EBULL;
        return x ^ (x >> 31U);
}

// The four 16-bit patterns of numerator, denominator, quotient and remainder, as one word.
[[nodiscard]] constexpr auto pack(std::uint16_t n, std::uint16_t d, std::uint16_t q, std::uint16_t r) noexcept
        -> std::uint64_t
{
        return (std::uint64_t{n} << 48U) | (std::uint64_t{d} << 32U) | (std::uint64_t{q} << 16U) | std::uint64_t{r};
}

// Whether a sweep found nothing, saying what it found otherwise.
[[nodiscard]] inline auto passed(char const* what, std::uint64_t failures)
        -> bool
{
        if (failures != 0U) {
                std::cerr << what << ": " << failures << " failures\n";
        }
        return failures == 0U;
}

} // namespace exhaustive

#endif // EXHAUSTIVE_SWEEP_HPP
