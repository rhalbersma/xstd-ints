//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <board.hpp>   // count, parse, position
#include <sliders.hpp> // bishop_directions, compressed_table, intrinsic_compress, magic_table, reversed_lines, rook_directions, walked_attacks, xstd_compress
#include <array>       // array
#include <chrono>      // duration, steady_clock
#include <cstdint>     // uint64_t
#include <cstdio>      // printf, puts
#include <random>      // mt19937_64
#include <string_view> // string_view
#include <vector>      // vector

namespace {

using perft::bitboard;

template<class Table>
struct sliders
{
        Table bishops{perft::bishop_directions};
        Table rooks{perft::rook_directions};

        [[nodiscard]] auto bishop(int sq, bitboard occupied) const noexcept
                -> bitboard
        {
                return bishops(sq, occupied);
        }

        [[nodiscard]] auto rook(int sq, bitboard occupied) const noexcept
                -> bitboard
        {
                return rooks(sq, occupied);
        }
};

struct expected
{
        std::string_view name;
        std::string_view fen;
        int quick_depth;
        std::uint64_t quick_nodes;
        int full_depth;
        std::uint64_t full_nodes;
};

// The chessprogramming.org perft suite: the start position, Kiwipete and positions 3 to 6, with their published counts.
constexpr auto suite = std::array{
        expected{"start", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", 4, 197'281, 6, 119'060'324},
        expected{"kiwipete", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 3, 97'862, 5, 193'690'690},
        expected{"position 3", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 5, 674'624, 6, 11'030'083},
        expected{"position 4", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -", 4, 422'333, 5, 15'833'292},
        expected{"position 5", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -", 3, 62'379, 5, 89'941'194},
        expected{"position 6", "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -", 3, 89'890, 5, 164'075'551},
};

// Every table against the square-by-square walk, on every square and on pseudo-random occupancies.
template<class Sliders>
[[nodiscard]] auto agrees(Sliders const& s)
        -> bool
{
        auto rng = std::mt19937_64{20261017U};
        for (auto i = 0; i < 4096; ++i) {
                auto const occupied = rng() & rng();
                for (auto sq = 0; sq < 64; ++sq) {
                        if (s.bishop(sq, occupied) != perft::walked_attacks(sq, occupied, perft::bishop_directions) or
                            s.rook(sq, occupied) != perft::walked_attacks(sq, occupied, perft::rook_directions)) {
                                return false;
                        }
                }
        }
        return true;
}

struct run
{
        std::uint64_t nodes;
        double seconds;
};

template<class Sliders>
[[nodiscard]] auto measure(Sliders const& s, expected const& e, bool full)
        -> run
{
        auto const start = std::chrono::steady_clock::now();
        auto const nodes = perft::count(perft::parse(e.fen), full ? e.full_depth : e.quick_depth, s);
        return {nodes, std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count()};
}

} // namespace

// Perft over the suite with each way of finding slider attacks; every count must equal the published one.
auto main(int argc, char** argv) -> int
{
        auto const full = argc > 1 and std::string_view{argv[1]} == "full";

        auto const by_compress = sliders<perft::compressed_table<perft::xstd_compress>>{};
        auto const by_reverse  = sliders<perft::reversed_lines>{};
        auto const by_magic    = sliders<perft::magic_table>{};
#ifdef __BMI2__
        auto const by_pext = sliders<perft::compressed_table<perft::intrinsic_compress>>{};
#endif
        if (not(agrees(by_compress) and agrees(by_reverse) and agrees(by_magic))) {
                std::puts("a slider table disagrees with the square-by-square walk");
                return 1;
        }

        std::printf("| position | depth | nodes | xstd::bit_compress | xstd::bit_reverse | magic |%s\n",
#ifdef __BMI2__
                    " _pext_u64 |"
#else
                    ""
#endif
        );
        std::printf("| :-- | --: | --: | --: | --: | --: |%s\n",
#ifdef __BMI2__
                    " --: |"
#else
                    ""
#endif
        );
        auto ok     = true;
        auto totals = std::array<run, 4>{};
        for (auto const& e : suite) {
                auto const want = full ? e.full_nodes : e.quick_nodes;
                auto runs       = std::vector<run>{measure(by_compress, e, full), measure(by_reverse, e, full), measure(by_magic, e, full)};
#ifdef __BMI2__
                runs.push_back(measure(by_pext, e, full));
#endif
                std::printf("| %.*s | %d | %llu |", static_cast<int>(e.name.size()), e.name.data(), full ? e.full_depth : e.quick_depth, static_cast<unsigned long long>(want));
                for (auto i = std::size_t{0}; i < runs.size(); ++i) {
                        ok = ok and runs[i].nodes == want;
                        totals[i].nodes += runs[i].nodes;
                        totals[i].seconds += runs[i].seconds;
                        if (runs[i].nodes == want) {
                                std::printf(" %.1f |", runs[i].nodes / runs[i].seconds * 1e-6);
                        } else {
                                std::printf(" **%llu** |", static_cast<unsigned long long>(runs[i].nodes));
                        }
                }
                std::puts("");
        }
        std::printf("| all | | |");
        for (auto i = std::size_t{0}; i < (sizeof(totals) / sizeof(totals[0])); ++i) {
                if (totals[i].seconds > 0) {
                        std::printf(" %.1f |", totals[i].nodes / totals[i].seconds * 1e-6);
                }
        }
        std::puts("");
        std::puts(ok ? "\nEvery count equals the published one; cells are millions of leaf nodes a second." : "\nA count differs from the published one, in bold.");
        return ok ? 0 : 1;
}
