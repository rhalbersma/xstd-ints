//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/detail/intrinsic/block_reverse.hpp> // IWYU pragma: keep; block_reverse and the macros naming which one it is
#include <xstd/ints/bit/detail/portable/block_compress.hpp> // block_compress_by_rank
#include <xstd/ints/bit/detail/portable/block_expand.hpp>   // block_expand_by_select, block_expand_by_walk
#include <xstd/ints/bit/detail/portable/block_reverse.hpp>  // block_reverse_by_swaps
#include <xstd/ints/bit/detail/portable/block_select.hpp>   // block_select_by_broadword
#include <alternatives.hpp>                                 // compress_by_walk_with_if, compress_by_parallel_suffix, expand_by_walk_with_if, expand_by_parallel_suffix, word_width
#include <machine.hpp>                                      // clock_ghz, compiler_name, cpu_name
#include <zp7.hpp>                                          // zp7_by_clmul, zp7_by_shifts
#include <nanobench.h>                                      // Bench, doNotOptimizeAway, Result
#include <algorithm>                                        // max, min, sort
#include <bit>                                              // countr_zero, popcount
#include <chrono>                                           // milliseconds
#include <cstddef>                                          // size_t
#include <cstdio>                                           // printf, puts
#include <limits>                                           // numeric_limits
#include <numeric>                                          // iota
#include <random>                                           // mt19937_64
#include <span>                                             // span
#include <string>                                           // string, to_string
#include <utility>                                          // move, pair, swap
#include <vector>                                           // vector

#ifdef __BMI2__
#include <immintrin.h> // _pdep_u64, _pext_u64
#endif

namespace {

using input = std::pair<std::size_t, std::size_t>;

constexpr auto calls  = 4096UZ;
constexpr auto passes = 5;
constexpr auto epochs = 11;

// The same inputs on every machine and every run, so that two tables differ only by what ran them.
auto rng = std::mt19937_64{20261017U};

// A word with exactly `ones` one-bits, at positions drawn without replacement.
[[nodiscard]] auto with_ones(unsigned ones)
        -> std::size_t
{
        auto positions = std::vector<unsigned>(bench::word_width);
        std::iota(positions.begin(), positions.end(), 0U);
        auto word = 0UZ;
        for (auto drawn = 0U; drawn < ones; ++drawn) {
                auto const pick = drawn + static_cast<unsigned>(rng() % (bench::word_width - drawn));
                std::swap(positions[drawn], positions[pick]);
                word |= 1UZ << positions[drawn];
        }
        return word;
}

// A word whose ones are `ones` of the ones of `within`, drawn without replacement.
[[nodiscard]] auto with_ones_within(std::size_t within, unsigned ones)
        -> std::size_t
{
        auto word = 0UZ;
        for (auto picked = 0U; picked < ones; ++picked) {
                auto candidates = within & ~word;
                for (auto skip = rng() % static_cast<unsigned>(std::popcount(candidates)); skip > 0U; --skip) {
                        candidates &= candidates - 1UZ;
                }
                word |= candidates & (0UZ - candidates);
        }
        return word;
}

// Keeps a result live without a store, so that neither the call nor the loop around it can be removed or vectorized.
template<class T>
auto keep(T& value) noexcept
        -> void
{
#if defined(__GNUC__)
        asm volatile("" : "+r"(value));
#else
        ankerl::nanobench::doNotOptimizeAway(value);
#endif
}

struct cost
{
        double nanoseconds  = std::numeric_limits<double>::infinity();
        double cycles       = std::numeric_limits<double>::infinity();
        double instructions = std::numeric_limits<double>::infinity();
};

// Independent calls over every input in turn: a call's throughput cost, the harness included, at its fastest run.
template<class Kernel>
[[nodiscard]] auto per_call(std::span<input const> inputs, Kernel kernel)
        -> cost
{
        auto bench = ankerl::nanobench::Bench().output(nullptr).performanceCounters(true).minEpochTime(std::chrono::milliseconds{5}).epochs(epochs);
        bench.run([&] -> void {
                for (auto const& [first, second] : inputs) {
                        auto result = kernel(first, second);
                        keep(result);
                }
        });
        using measure     = ankerl::nanobench::Result::Measure;
        auto const& found = bench.results().back();
        auto const n      = static_cast<double>(inputs.size());
        auto measured     = cost{.nanoseconds = found.minimum(measure::elapsed) * 1e9 / n};
        if (found.has(measure::cpucycles) and found.has(measure::instructions)) {
                measured.cycles       = found.minimum(measure::cpucycles) / n;
                measured.instructions = found.minimum(measure::instructions) / n;
        }
        return measured;
}

// Each kernel is a captureless lambda, so its type alone rebuilds it and a plain function pointer can carry it.
template<class Kernel>
[[nodiscard]] auto run(std::span<input const> inputs)
        -> cost
{
        return per_call(inputs, Kernel{});
}

template<class Kernel>
[[nodiscard]] auto result(input in)
        -> std::size_t
{
        return static_cast<std::size_t>(Kernel{}(in.first, in.second));
}

struct column
{
        std::string name;
        auto (*run)(std::span<input const>) -> cost;
        auto (*result)(input) -> std::size_t;
};

template<class Kernel>
[[nodiscard]] auto make_column(std::string name, Kernel)
        -> column
{
        return {.name = std::move(name), .run = &run<Kernel>, .result = &result<Kernel>};
}

struct row
{
        std::string label;
        std::vector<input> inputs;
};

// A row per input distribution, a column per kernel, and per cell the fastest of every pass and the slowest.
struct table
{
        std::string title;
        std::vector<row> rows;
        std::vector<column> columns;
        std::vector<std::vector<cost>> fastest   = {};
        std::vector<std::vector<double>> slowest = {};
};

[[nodiscard]] auto compress_table()
        -> table
{
        namespace portable = xstd::ints::detail::portable;
        auto rows          = std::vector<row>{};
        for (auto const ones : {4U, 8U, 16U, 32U, 48U, 64U}) {
                auto inputs = std::vector<input>{};
                for (auto i = 0UZ; i < calls; ++i) {
                        auto const mask = with_ones(ones);
                        inputs.emplace_back(with_ones_within(mask, ones / 2U) | (rng() & ~mask), mask);
                }
                rows.push_back({std::to_string(ones) + " mask ones, " + std::to_string(ones / 2U) + " selected", std::move(inputs)});
        }
        auto columns = std::vector<column>{
                make_column("by rank", [](std::size_t s, std::size_t m) -> std::size_t { return portable::block_compress_by_rank(s, m); }),
#ifdef __BMI2__
                make_column("PEXT", [](std::size_t s, std::size_t m) -> std::size_t { return _pext_u64(s, m); }),
#endif
                make_column("walk with `if`", [](std::size_t s, std::size_t m) -> std::size_t { return bench::compress_by_walk_with_if(s, m); }),
                make_column("parallel suffix", [](std::size_t s, std::size_t m) -> std::size_t { return bench::compress_by_parallel_suffix(s, m); }),
                make_column("zp7", [](std::size_t s, std::size_t m) -> std::size_t { return bench::zp7_by_shifts::zp7_pext_64(s, m); }),
#ifdef __PCLMUL__
                make_column("zp7, PCLMULQDQ", [](std::size_t s, std::size_t m) -> std::size_t { return bench::zp7_by_clmul::zp7_pext_64(s, m); }),
#endif
        };
        return {.title = "`bit_compress` on one 64-bit block", .rows = std::move(rows), .columns = std::move(columns)};
}

[[nodiscard]] auto expand_table()
        -> table
{
        namespace portable = xstd::ints::detail::portable;
        auto rows          = std::vector<row>{};
        for (auto const ones : {4U, 8U, 16U, 32U, 48U, 64U}) {
                auto const source_counts = ones / 2U > 2U ? std::vector{2U, ones / 2U} : std::vector{2U};
                for (auto const source_ones : source_counts) {
                        auto inputs = std::vector<input>{};
                        for (auto i = 0UZ; i < calls; ++i) {
                                auto const low = ones == bench::word_width ? ~0UZ : (1UZ << ones) - 1UZ;
                                inputs.emplace_back(with_ones_within(low, source_ones) | (rng() & ~low), with_ones(ones));
                        }
                        rows.push_back({std::to_string(ones) + " mask ones, " + std::to_string(source_ones) + " source ones", std::move(inputs)});
                }
        }
        auto columns = std::vector<column>{
                make_column("by walk", [](std::size_t s, std::size_t m) -> std::size_t { return portable::block_expand_by_walk(s, m); }),
                make_column("by select", [](std::size_t s, std::size_t m) -> std::size_t { return portable::block_expand_by_select(s, m); }),
#ifdef __BMI2__
                make_column("PDEP", [](std::size_t s, std::size_t m) -> std::size_t { return _pdep_u64(s, m); }),
#endif
                make_column("walk with `if`", [](std::size_t s, std::size_t m) -> std::size_t { return bench::expand_by_walk_with_if(s, m); }),
                make_column("parallel suffix", [](std::size_t s, std::size_t m) -> std::size_t { return bench::expand_by_parallel_suffix(s, m); }),
                make_column("zp7", [](std::size_t s, std::size_t m) -> std::size_t { return bench::zp7_by_shifts::zp7_pdep_64(s, m); }),
#ifdef __PCLMUL__
                make_column("zp7, PCLMULQDQ", [](std::size_t s, std::size_t m) -> std::size_t { return bench::zp7_by_clmul::zp7_pdep_64(s, m); }),
#endif
        };
        return {.title = "`bit_expand` on one 64-bit block, its source ones counted among the mask's width", .rows = std::move(rows), .columns = std::move(columns)};
}

[[nodiscard]] auto select_table()
        -> table
{
        namespace portable = xstd::ints::detail::portable;
        auto rows          = std::vector<row>{};
        for (auto const ones : {8U, 32U, 64U}) {
                auto inputs = std::vector<input>{};
                for (auto i = 0UZ; i < calls; ++i) {
                        inputs.emplace_back(with_ones(ones), rng() % ones);
                }
                rows.push_back({std::to_string(ones) + " ones, any rank", std::move(inputs)});
        }
        auto columns = std::vector<column>{
                make_column("by broadword", [](std::size_t x, std::size_t k) -> std::size_t { return portable::block_select_by_broadword(x, static_cast<unsigned>(k)); }),
#ifdef __BMI2__
                make_column("PDEP, TZCNT", [](std::size_t x, std::size_t k) -> std::size_t { return static_cast<std::size_t>(std::countr_zero(_pdep_u64(1ULL << k, x))); }),
#endif
        };
        return {.title = "`bit_select` on one 64-bit block", .rows = std::move(rows), .columns = std::move(columns)};
}

[[nodiscard]] auto reverse_table()
        -> table
{
        namespace portable = xstd::ints::detail::portable;
        auto inputs        = std::vector<input>{};
        for (auto i = 0UZ; i < calls; ++i) {
                inputs.emplace_back(rng(), 0UZ);
        }
        auto rows    = std::vector<row>{{"any word", std::move(inputs)}};
        auto columns = std::vector<column>{
                make_column("by swaps", [](std::size_t x, std::size_t) -> std::size_t { return portable::block_reverse_by_swaps(x); }),
#if defined(XSTD_INTS_HAS_BUILTIN_BITREVERSE)
                make_column("`__builtin_bitreverse64`", [](std::size_t x, std::size_t) -> std::size_t { return xstd::ints::detail::intrinsic::block_reverse(x); }),
#elif defined(XSTD_INTS_HAS_RBIT)
                make_column("RBIT", [](std::size_t x, std::size_t) -> std::size_t { return xstd::ints::detail::intrinsic::block_reverse(x); }),
#elif defined(XSTD_INTS_HAS_GFNI)
                make_column("GF2P8AFFINEQB, BSWAP", [](std::size_t x, std::size_t) -> std::size_t { return xstd::ints::detail::intrinsic::block_reverse(x); }),
#endif
        };
        return {.title = "`bit_reverse` on one 64-bit block", .rows = std::move(rows), .columns = std::move(columns)};
}

// The harness alone: a kernel that only XORs its two words.
[[nodiscard]] auto harness_table()
        -> table
{
        auto inputs = std::vector<input>{};
        for (auto i = 0UZ; i < calls; ++i) {
                inputs.emplace_back(rng(), rng());
        }
        auto rows    = std::vector<row>{{"any two words", std::move(inputs)}};
        auto columns = std::vector<column>{make_column("XOR", [](std::size_t s, std::size_t m) -> std::size_t { return s ^ m; })};
        return {.title = "The harness alone", .rows = std::move(rows), .columns = std::move(columns)};
}

// Every kernel against the table's first, on every input it is timed on.
[[nodiscard]] auto agrees(table const& t)
        -> bool
{
        for (auto const& r : t.rows) {
                for (auto const& in : r.inputs) {
                        auto const expected = t.columns.front().result(in);
                        for (auto const& c : t.columns) {
                                if (c.result(in) != expected) {
                                        std::printf("%s disagrees with %s on %#zx, %#zx\n", c.name.c_str(), t.columns.front().name.c_str(), in.first, in.second);
                                        return false;
                                }
                        }
                }
        }
        return true;
}

// One pass times every cell once; a cell keeps its fastest pass, so a neighbour's burst costs a pass, not the table.
auto measure(table& t)
        -> void
{
        t.fastest.resize(t.rows.size(), std::vector<cost>(t.columns.size()));
        t.slowest.resize(t.rows.size(), std::vector<double>(t.columns.size(), 0.0));
        for (auto r = 0UZ; r < t.rows.size(); ++r) {
                for (auto c = 0UZ; c < t.columns.size(); ++c) {
                        auto const measured = t.columns[c].run(t.rows[r].inputs);
                        auto& best          = t.fastest[r][c];
                        best.nanoseconds    = std::min(best.nanoseconds, measured.nanoseconds);
                        best.cycles         = std::min(best.cycles, measured.cycles);
                        best.instructions   = std::min(best.instructions, measured.instructions);
                        t.slowest[r][c]     = std::max(t.slowest[r][c], measured.nanoseconds);
                }
        }
}

auto print(table const& t, double ghz, bool counted)
        -> void
{
        std::printf("\n### %s\n\n| inputs |", t.title.c_str());
        for (auto const& c : t.columns) {
                std::printf(" %s |", c.name.c_str());
        }
        std::printf("\n| :-- |");
        for (auto c = 0UZ; c < t.columns.size(); ++c) {
                std::printf(" --: |");
        }
        std::puts("");
        for (auto r = 0UZ; r < t.rows.size(); ++r) {
                std::printf("| %s |", t.rows[r].label.c_str());
                for (auto const& cell : t.fastest[r]) {
                        if (counted) {
                                std::printf(" %.1f / %.0f |", cell.cycles, cell.instructions);
                        } else {
                                std::printf(" %.1f |", cell.nanoseconds * ghz);
                        }
                }
                std::puts("");
        }
}

} // namespace

auto main() -> int
{
        auto tables = std::vector<table>{compress_table(), expand_table(), select_table(), reverse_table(), harness_table()};
        for (auto const& t : tables) {
                if (not agrees(t)) {
                        return 1;
                }
        }

        auto ghz = 0.0;
        for (auto pass = 0; pass < passes; ++pass) {
                ghz = std::max(ghz, bench::clock_ghz());
                for (auto& t : tables) {
                        measure(t);
                }
        }
        auto const counted = tables.front().fastest.front().front().cycles != std::numeric_limits<double>::infinity();

        auto spreads = std::vector<double>{};
        for (auto const& t : tables) {
                for (auto r = 0UZ; r < t.rows.size(); ++r) {
                        for (auto c = 0UZ; c < t.columns.size(); ++c) {
                                spreads.push_back(t.slowest[r][c] / t.fastest[r][c].nanoseconds - 1.0);
                        }
                }
        }
        std::ranges::sort(spreads);

        std::printf("## %s\n\n", bench::compiler_name().c_str());
        std::printf("- **Machine:** %s\n", bench::cpu_name().c_str());
        std::printf("- **Flags:** `%s`\n", XSTD_INTS_BENCH_FLAGS);
        std::printf("- **Commit:** `%s`\n", XSTD_INTS_BENCH_COMMIT);
        if (counted) {
                std::puts("- **Cells:** cycles / instructions per call, both from the performance counters");
        } else {
                std::printf("- **Cells:** cycles per call, from time at %.2f GHz, the fastest clock a chain of dependent adds ran at\n", ghz);
        }
        std::printf("- **Calls:** %zu independent calls on fixed pseudo-random inputs; the fastest of %d passes of %d runs each\n", calls, passes, epochs);
        std::printf("- **Noise:** a cell's slowest pass took %.0f%% longer than its fastest at the median, %.0f%% at the most\n", spreads[spreads.size() / 2U] * 100.0, spreads.back() * 100.0);
        for (auto const& t : tables) {
                print(t, ghz, counted);
        }
        return 0;
}
