//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_EXAMPLES_PERFT_BOARD_HPP
#define XSTD_INTS_EXAMPLES_PERFT_BOARD_HPP

#include <xstd/ints/bit/countr_zero.hpp> // countr_zero
#include <sliders.hpp>                   // bishop_directions, bit, bitboard, rook_directions, square, walked_attacks
#include <array>                         // array
#include <cstddef>                       // size_t
#include <cstdint>                       // uint8_t
#include <sstream>                       // istringstream
#include <string>                        // string
#include <string_view>                   // string_view
#include <utility>                       // pair

// A chess position, its pseudo-legal moves, and copy-make, over bitboards whose squares run a1 = 0 to h8 = 63.
namespace perft {

enum piece : int { pawn,
                   knight,
                   bishop,
                   rook,
                   queen,
                   king };

inline constexpr auto white = 0;
inline constexpr auto black = 1;

struct position
{
        std::array<std::array<bitboard, 6>, 2> pieces{};
        std::array<bitboard, 2> colour{};
        int side        = white;
        unsigned rights = 0; // 1 white short, 2 white long, 4 black short, 8 black long
        int en_passant  = -1;
};

struct move
{
        std::uint8_t from;
        std::uint8_t to;
        std::uint8_t promotion; // a piece, or pawn for none
        std::uint8_t kind;      // 0 quiet or capture, 1 double push, 2 en passant, 3 castling
};

[[nodiscard]] inline auto parse(std::string_view fen)
        -> position
{
        auto p      = position{};
        auto fields = std::istringstream{std::string{fen}};
        auto board  = std::string{};
        auto side   = std::string{};
        auto rights = std::string{};
        auto ep     = std::string{};
        fields >> board >> side >> rights >> ep;
        auto rank = 7;
        auto file = 0;
        for (auto const c : board) {
                if (c == '/') {
                        --rank;
                        file = 0;
                } else if ('1' <= c and c <= '8') {
                        file += c - '0';
                } else {
                        auto const colour = ('a' <= c and c <= 'z') ? black : white;
                        auto const kind   = std::string_view{"pnbrqk"}.find(static_cast<char>(c | 0x20));
                        p.pieces[colour][kind] |= bit(square(file, rank));
                        p.colour[colour] |= bit(square(file, rank));
                        ++file;
                }
        }
        p.side = side == "b" ? black : white;
        for (auto const c : rights) {
                p.rights |= c == 'K' ? 1U : c == 'Q' ? 2U
                                    : c == 'k'       ? 4U
                                    : c == 'q'       ? 8U
                                                     : 0U;
        }
        p.en_passant = ep == "-" ? -1 : square(ep[0] - 'a', ep[1] - '1');
        return p;
}

// The squares a knight, a king, or a pawn of either colour attacks from each square.
struct leapers
{
        std::array<bitboard, 64> knight{};
        std::array<bitboard, 64> king{};
        std::array<std::array<bitboard, 64>, 2> pawn{};

        leapers()
        {
                for (auto sq = 0; sq < 64; ++sq) {
                        auto const f = sq % 8;
                        auto const r = sq / 8;
                        for (auto const& [df, dr] : {std::pair{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}}) {
                                knight[sq] |= on_board(f + df, r + dr);
                        }
                        for (auto const& [df, dr] : {std::pair{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}}) {
                                king[sq] |= on_board(f + df, r + dr);
                        }
                        pawn[white][sq] = on_board(f - 1, r + 1) | on_board(f + 1, r + 1);
                        pawn[black][sq] = on_board(f - 1, r - 1) | on_board(f + 1, r - 1);
                }
        }

        [[nodiscard]] static auto on_board(int f, int r) noexcept
                -> bitboard
        {
                return (0 <= f and f < 8 and 0 <= r and r < 8) ? bit(square(f, r)) : 0;
        }
};

inline leapers const leaping{};

// Whether a piece of colour `by` attacks the square, with the bishop and rook attacks Sliders finds.
template<class Sliders>
[[nodiscard]] auto attacked(position const& p, int sq, int by, Sliders const& sliders) noexcept
        -> bool
{
        auto const& them    = p.pieces[by];
        auto const occupied = p.colour[white] | p.colour[black];
        return (leaping.pawn[1 - by][sq] & them[pawn]) != 0 or (leaping.knight[sq] & them[knight]) != 0 or (leaping.king[sq] & them[king]) != 0 or
               (sliders.bishop(sq, occupied) & (them[bishop] | them[queen])) != 0 or (sliders.rook(sq, occupied) & (them[rook] | them[queen])) != 0;
}

// Calls f with each square of the bitboard, lowest first.
template<class F>
auto for_each_square(bitboard b, F f)
        -> void
{
        for (; b != 0; b &= b - 1) {
                f(xstd::countr_zero(b));
        }
}

template<class Sliders>
[[nodiscard]] auto generate(position const& p, Sliders const& sliders, std::array<move, 256>& moves)
        -> std::size_t
{
        auto n          = std::size_t{0};
        auto const us   = p.side;
        auto const own  = p.colour[us];
        auto const opp  = p.colour[1 - us];
        auto const occ  = own | opp;
        auto const push = us == white ? 8 : -8;
        auto add        = [&](int from, int to, int promotion, int kind) -> void {
                moves[n++] = {static_cast<std::uint8_t>(from), static_cast<std::uint8_t>(to), static_cast<std::uint8_t>(promotion), static_cast<std::uint8_t>(kind)};
        };
        auto add_pawn = [&](int from, int to, int kind) -> void {
                if (to / 8 == 0 or to / 8 == 7) {
                        for (auto const promotion : {queen, rook, bishop, knight}) {
                                add(from, to, promotion, kind);
                        }
                } else {
                        add(from, to, pawn, kind);
                }
        };
        for_each_square(p.pieces[us][pawn], [&](int from) -> void {
                auto const to = from + push;
                if ((occ & bit(to)) == 0) {
                        add_pawn(from, to, 0);
                        auto const start = us == white ? 1 : 6;
                        if (from / 8 == start and (occ & bit(to + push)) == 0) {
                                add(from, to + push, pawn, 1);
                        }
                }
                for_each_square(leaping.pawn[us][from] & opp, [&](int target) -> void { add_pawn(from, target, 0); });
                if (p.en_passant >= 0 and (leaping.pawn[us][from] & bit(p.en_passant)) != 0) {
                        add(from, p.en_passant, pawn, 2);
                }
        });
        auto add_targets = [&](int from, bitboard targets) -> void {
                for_each_square(targets & ~own, [&](int to) -> void { add(from, to, pawn, 0); });
        };
        for_each_square(p.pieces[us][knight], [&](int from) -> void { add_targets(from, leaping.knight[from]); });
        for_each_square(p.pieces[us][bishop], [&](int from) -> void { add_targets(from, sliders.bishop(from, occ)); });
        for_each_square(p.pieces[us][rook], [&](int from) -> void { add_targets(from, sliders.rook(from, occ)); });
        for_each_square(p.pieces[us][queen], [&](int from) -> void { add_targets(from, sliders.bishop(from, occ) | sliders.rook(from, occ)); });
        for_each_square(p.pieces[us][king], [&](int from) -> void {
                add_targets(from, leaping.king[from]);
                // The king may not castle out of or through check; landing in check is caught like any other move.
                auto const home = us == white ? 0 : 56;
                if (from == home + 4 and not attacked(p, from, 1 - us, sliders)) {
                        auto const short_right = us == white ? 1U : 4U;
                        if ((p.rights & short_right) != 0 and (occ & (bit(home + 5) | bit(home + 6))) == 0 and not attacked(p, home + 5, 1 - us, sliders)) {
                                add(from, home + 6, pawn, 3);
                        }
                        auto const long_right = us == white ? 2U : 8U;
                        if ((p.rights & long_right) != 0 and (occ & (bit(home + 1) | bit(home + 2) | bit(home + 3))) == 0 and not attacked(p, home + 3, 1 - us, sliders)) {
                                add(from, home + 2, pawn, 3);
                        }
                }
        });
        return n;
}

// Moving from or to a square keeps only the castling rights this mask keeps: a king's or a rook's home square.
inline constexpr auto rights_kept = [] -> std::array<unsigned, 64> {
        auto kept = std::array<unsigned, 64>{};
        kept.fill(15U);
        kept[0]  = 15U & ~2U;
        kept[4]  = 15U & ~3U;
        kept[7]  = 15U & ~1U;
        kept[56] = 15U & ~8U;
        kept[60] = 15U & ~12U;
        kept[63] = 15U & ~4U;
        return kept;
}();

[[nodiscard]] inline auto make(position p, move m) noexcept
        -> position
{
        auto const us   = p.side;
        auto const them = 1 - us;
        auto const from = bit(m.from);
        auto const to   = bit(m.to);
        for (auto& b : p.pieces[them]) {
                b &= ~to;
        }
        p.colour[them] &= ~to;
        auto moved = pawn;
        for (auto kind = pawn; kind <= king; kind = static_cast<piece>(kind + 1)) {
                if ((p.pieces[us][kind] & from) != 0) {
                        moved = kind;
                }
        }
        p.pieces[us][moved] &= ~from;
        p.pieces[us][m.promotion != pawn ? static_cast<piece>(m.promotion) : moved] |= to;
        p.colour[us] = (p.colour[us] & ~from) | to;
        if (m.kind == 2) {
                auto const captured = bit(us == white ? m.to - 8 : m.to + 8);
                p.pieces[them][pawn] &= ~captured;
                p.colour[them] &= ~captured;
        } else if (m.kind == 3) {
                auto const rook_from = bit(m.to > m.from ? m.from + 3 : m.from - 4);
                auto const rook_to   = bit(m.to > m.from ? m.from + 1 : m.from - 1);
                p.pieces[us][rook]   = (p.pieces[us][rook] & ~rook_from) | rook_to;
                p.colour[us]         = (p.colour[us] & ~rook_from) | rook_to;
        }
        p.rights &= rights_kept[m.from] & rights_kept[m.to];
        p.en_passant = m.kind == 1 ? (m.from + m.to) / 2 : -1;
        p.side       = them;
        return p;
}

// The leaf nodes of the legal move tree to the given depth: a pseudo-legal move that leaves the king attacked is none.
template<class Sliders>
[[nodiscard]] auto count(position const& p, int depth, Sliders const& sliders)
        -> std::uint64_t
{
        auto moves   = std::array<move, 256>{};
        auto const n = generate(p, sliders, moves);
        auto nodes   = std::uint64_t{0};
        for (auto i = std::size_t{0}; i < n; ++i) {
                auto const child = make(p, moves[i]);
                if (not attacked(child, xstd::countr_zero(child.pieces[p.side][king]), child.side, sliders)) {
                        nodes += depth == 1 ? 1 : count(child, depth - 1, sliders);
                }
        }
        return nodes;
}

} // namespace perft

#endif // XSTD_INTS_EXAMPLES_PERFT_BOARD_HPP
