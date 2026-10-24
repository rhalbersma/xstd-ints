//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/countr_zero.hpp> // countr_zero
#include <select_dictionary.hpp>         // select_dictionary
#include <sdsl/bit_vectors.hpp>          // bit_vector
#include <sdsl/select_support_mcl.hpp>   // select_support_mcl
#include <sux/bits/Rank9Sel.hpp>         // Rank9Sel
#include <algorithm>                     // min
#include <chrono>                        // duration, steady_clock
#include <cstddef>                       // size_t
#include <cstdint>                       // uint64_t
#include <cstdio>                        // fflush, printf, puts
#include <cstdlib>                       // strtoull
#include <random>                        // mt19937_64
#include <string>                        // string, to_string
#include <utility>                       // pair
#include <vector>                        // vector

namespace {

// SplitMix64, so that a billion bits draw quickly and identically on every run.
struct splitmix
{
        std::uint64_t state;

        [[nodiscard]] auto operator()() noexcept
                -> std::uint64_t
        {
                auto z = (state += 0x9e37'79b9'7f4a'7c15ULL);
                z      = (z ^ (z >> 30U)) * 0xbf58'476d'1ce4'e5b9ULL;
                z      = (z ^ (z >> 27U)) * 0x94d0'49bb'1331'11ebULL;
                return z ^ (z >> 31U);
        }
};

// Each bit a one with the given probability, or, when clustered, runs of 90% and of 1% averaging 4096 bits each.
[[nodiscard]] auto draw(std::size_t bits, double density, bool clustered)
        -> std::vector<std::uint64_t>
{
        auto rng   = splitmix{20261017U};
        auto words = std::vector<std::uint64_t>((bits + 63) / 64);
        auto dense = true;
        auto left  = std::uint64_t{0};
        for (auto i = std::size_t{0}; i < bits; ++i) {
                if (clustered and left-- == 0) {
                        dense = not dense;
                        left  = rng() % 8192;
                }
                auto const p         = clustered ? (dense ? 0.9 : 0.01) : density;
                auto const threshold = static_cast<std::uint64_t>(p * 18446744073709551616.0);
                if (rng() < threshold) {
                        words[i / 64] |= std::uint64_t{1} << (i % 64);
                }
        }
        return words;
}

// Every round's answers end up here, so that no compiler drops a round whose answers nothing else reads.
volatile std::uint64_t answers_folded = 0;

template<class F>
[[nodiscard]] auto nanoseconds_per_query(std::vector<std::uint64_t> const& ranks, std::vector<std::uint64_t>& answers, F select)
        -> double
{
        auto const start = std::chrono::steady_clock::now();
        for (auto i = std::size_t{0}; i < ranks.size(); ++i) {
                answers[i] = select(ranks[i]);
        }
        auto const stop = std::chrono::steady_clock::now();
        auto folded     = std::uint64_t{0};
        for (auto const answer : answers) {
                folded ^= answer;
        }
        answers_folded = folded;
        return std::chrono::duration<double, std::nano>(stop - start).count() / static_cast<double>(ranks.size());
}

} // namespace

// Select throughput of xstd's dictionary against sux's Rank9Sel and SDSL's select_support_mcl, every answer compared.
auto main(int argc, char** argv) -> int
{
        auto const bits    = static_cast<std::size_t>(argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 1ULL << 30U);
        auto const queries = std::size_t{4'000'000};
        std::printf("| bits | distribution | ones | xstd | sux `Rank9Sel` | SDSL `select_support_mcl` | xstd, its last step a `countr_zero` |\n");
        std::puts("| --: | :-- | --: | --: | --: | --: | --: |");
        auto ok = true;
        for (auto const& [density, clustered] : {std::pair{0.01, false}, {0.05, false}, {0.10, false}, {0.25, false}, {0.50, false}, {0.75, false}, {0.90, false}, {0.0, true}}) {
                auto const words           = draw(bits, density, clustered);
                auto const xstd_dictionary = succinct::select_dictionary{words};
                auto sux_dictionary        = sux::bits::Rank9Sel<>{words.data(), bits}; // its select is not const
                auto sdsl_bits             = sdsl::bit_vector(bits);
                for (auto i = std::size_t{0}; i < words.size(); ++i) {
                        sdsl_bits.set_int(64 * i, words[i], static_cast<std::uint8_t>(std::min<std::size_t>(64, bits - 64 * i)));
                }
                auto const sdsl_dictionary = sdsl::select_support_mcl<1>{&sdsl_bits};

                auto rng   = std::mt19937_64{20261017U};
                auto ranks = std::vector<std::uint64_t>(queries);
                for (auto& rank : ranks) {
                        rank = rng() % xstd_dictionary.ones();
                }
                auto xstd_answers = std::vector<std::uint64_t>(queries);
                auto sux_answers  = std::vector<std::uint64_t>(queries);
                auto sdsl_answers = std::vector<std::uint64_t>(queries);
                auto scratch      = std::vector<std::uint64_t>(queries);
                // Five rounds, each structure once per round, and each keeps its fastest: a neighbour's burst costs a round.
                auto xstd_ns = 1e300;
                auto sux_ns  = 1e300;
                auto sdsl_ns = 1e300;
                auto scan_ns = 1e300;
                for (auto round = 0; round < 5; ++round) {
                        xstd_ns = std::min(xstd_ns, nanoseconds_per_query(ranks, xstd_answers, [&](std::uint64_t k) -> std::uint64_t { return xstd_dictionary.select(k); }));
                        sux_ns  = std::min(sux_ns, nanoseconds_per_query(ranks, sux_answers, [&](std::uint64_t k) -> std::uint64_t { return sux_dictionary.select(k); }));
                        sdsl_ns = std::min(sdsl_ns, nanoseconds_per_query(ranks, sdsl_answers, [&](std::uint64_t k) -> std::uint64_t { return sdsl_dictionary.select(k + 1); }));
                        // The same search with the in-word step replaced by one countr_zero: the difference is what bit_select costs.
                        scan_ns = std::min(scan_ns, nanoseconds_per_query(ranks, scratch, [&](std::uint64_t k) -> std::uint64_t {
                                                   return xstd_dictionary.select(k, [](std::uint64_t w, std::uint64_t) noexcept -> std::uint64_t { return static_cast<std::uint64_t>(xstd::countr_zero(w)); });
                                           }));
                }
                auto rank_agrees = true;
                for (auto i = std::size_t{0}; i < queries; ++i) {
                        auto const pos = rng() % bits;
                        rank_agrees    = rank_agrees and xstd_dictionary.rank(xstd_answers[i]) == ranks[i] and xstd_dictionary.rank(pos) == sux_dictionary.rank(pos);
                }
                auto const agree = xstd_answers == sux_answers and xstd_answers == sdsl_answers and rank_agrees;
                ok               = ok and agree;
                auto const name  = clustered ? std::string{"clustered"} : std::to_string(static_cast<int>(density * 100.0 + 0.5)) + "%";
                std::printf("| %zu | %s | %llu | %.1f | %.1f | %.1f | %.1f |%s\n", bits, name.c_str(), static_cast<unsigned long long>(xstd_dictionary.ones()), xstd_ns, sux_ns, sdsl_ns, scan_ns, agree ? "" : " answers differ");
                std::fflush(stdout);
        }
        std::puts(ok ? "\nEvery select agrees across the three, and every rank with sux and with select; cells are nanoseconds per select of a random rank, the fastest of five rounds." : "\nAnswers differ.");
        return ok ? 0 : 1;
}
