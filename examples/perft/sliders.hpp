//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_EXAMPLES_PERFT_SLIDERS_HPP
#define XSTD_INTS_EXAMPLES_PERFT_SLIDERS_HPP

#include <xstd/ints/bit/bit_compress.hpp> // bit_compress
#include <xstd/ints/bit/bit_expand.hpp>   // bit_expand
#include <xstd/ints/bit/bit_reverse.hpp>  // bit_reverse
#include <xstd/ints/bit/popcount.hpp>     // popcount
#include <array>                          // array
#include <bit>                            // countr_zero, popcount
#include <cstddef>                        // size_t
#include <cstdint>                        // uint64_t
#include <random>                         // mt19937_64
#include <string_view>                    // string_view
#include <utility>                        // pair
#include <vector>                         // vector

#ifdef __BMI2__
#include <immintrin.h> // _pext_u64
#endif

// Three ways to find the squares a bishop or rook attacks, each built on a different bit primitive.
namespace perft {

using bitboard = std::uint64_t;

[[nodiscard]] constexpr auto square(int file, int rank) noexcept
        -> int
{
        return 8 * rank + file;
}

[[nodiscard]] constexpr auto bit(int sq) noexcept
        -> bitboard
{
        return bitboard{1} << sq;
}

using directions = std::array<std::pair<int, int>, 4>;

inline constexpr auto rook_directions   = directions{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
inline constexpr auto bishop_directions = directions{{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

// Each ray walked to its first blocker, square by square: the reference every table below is filled from.
[[nodiscard]] constexpr auto walked_attacks(int sq, bitboard occupied, directions const& rays) noexcept
        -> bitboard
{
        auto attacks = bitboard{0};
        for (auto const& [df, dr] : rays) {
                for (auto f = sq % 8 + df, r = sq / 8 + dr; 0 <= f and f < 8 and 0 <= r and r < 8; f += df, r += dr) {
                        attacks |= bit(square(f, r));
                        if ((occupied & bit(square(f, r))) != 0) {
                                break;
                        }
                }
        }
        return attacks;
}

// The squares whose occupancy decides the attacks: each ray less its last square, which nothing lies beyond.
[[nodiscard]] constexpr auto relevant(int sq, directions const& rays) noexcept
        -> bitboard
{
        auto mask = bitboard{0};
        for (auto const& [df, dr] : rays) {
                for (auto f = sq % 8 + df, r = sq / 8 + dr; 0 <= f + df and f + df < 8 and 0 <= r + dr and r + dr < 8; f += df, r += dr) {
                        mask |= bit(square(f, r));
                }
        }
        return mask;
}

// A table per square indexed by the relevant occupancy compressed to its low bits, as Stockfish's BMI2 path indexes it.
template<class Compress>
class compressed_table
{
public:
        explicit compressed_table(directions const& rays)
        {
                for (auto sq = 0; sq < 64; ++sq) {
                        m_mask[sq]   = relevant(sq, rays);
                        m_offset[sq] = m_attacks.size();
                        for (auto i = bitboard{0}; i < (bitboard{1} << xstd::popcount(m_mask[sq])); ++i) {
                                m_attacks.push_back(walked_attacks(sq, xstd::bit_expand(i, m_mask[sq]), rays));
                        }
                }
        }

        [[nodiscard]] auto operator()(int sq, bitboard occupied) const noexcept
                -> bitboard
        {
                return m_attacks[m_offset[sq] + static_cast<std::size_t>(Compress{}(occupied, m_mask[sq]))];
        }

private:
        std::array<bitboard, 64> m_mask{};
        std::array<std::size_t, 64> m_offset{};
        std::vector<bitboard> m_attacks;
};

struct xstd_compress
{
        [[nodiscard]] auto operator()(bitboard x, bitboard m) const noexcept
                -> bitboard
        {
                return xstd::bit_compress(x, m);
        }
};

#ifdef __BMI2__
struct intrinsic_compress
{
        [[nodiscard]] auto operator()(bitboard x, bitboard m) const noexcept
                -> bitboard
        {
                return _pext_u64(x, m);
        }
};
#endif

// Classic magic bitboards: the relevant occupancy times a found constant, shifted, indexes the table; no xstd call.
class magic_table
{
public:
        explicit magic_table(directions const& rays)
        {
                auto rng = std::mt19937_64{20261017U};
                for (auto sq = 0; sq < 64; ++sq) {
                        m_mask[sq]   = relevant(sq, rays);
                        auto const n = std::popcount(m_mask[sq]);
                        m_shift[sq]  = 64 - n;
                        m_offset[sq] = m_attacks.size();
                        auto subsets = std::vector<bitboard>{};
                        auto subset  = bitboard{0};
                        do {
                                subsets.push_back(subset);
                                subset = (subset - m_mask[sq]) & m_mask[sq];
                        } while (subset != 0);
                        auto table = std::vector<bitboard>(subsets.size());
                        for (;;) {
                                auto const candidate = rng() & rng() & rng();
                                if (std::popcount((m_mask[sq] * candidate) >> 56) < 6) {
                                        continue;
                                }
                                std::ranges::fill(table, ~bitboard{0});
                                auto collides = false;
                                for (auto const occupied : subsets) {
                                        auto const index    = static_cast<std::size_t>((occupied * candidate) >> m_shift[sq]);
                                        auto const attacked = walked_attacks(sq, occupied, rays);
                                        if (table[index] != ~bitboard{0} and table[index] != attacked) {
                                                collides = true;
                                                break;
                                        }
                                        table[index] = attacked;
                                }
                                if (not collides) {
                                        m_magic[sq] = candidate;
                                        break;
                                }
                        }
                        m_attacks.insert(m_attacks.end(), table.begin(), table.end());
                }
        }

        [[nodiscard]] auto operator()(int sq, bitboard occupied) const noexcept
                -> bitboard
        {
                return m_attacks[m_offset[sq] + static_cast<std::size_t>(((occupied & m_mask[sq]) * m_magic[sq]) >> m_shift[sq])];
        }

private:
        std::array<bitboard, 64> m_mask{};
        std::array<bitboard, 64> m_magic{};
        std::array<int, 64> m_shift{};
        std::array<std::size_t, 64> m_offset{};
        std::vector<bitboard> m_attacks;
};

// Hyperbola quintessence: o - 2s finds the attacks towards the high end of a line, its mirror image the other way.
class reversed_lines
{
public:
        explicit reversed_lines(directions const& rays)
        {
                for (auto sq = 0; sq < 64; ++sq) {
                        for (auto line = 0; line < 2; ++line) {
                                auto const& [df, dr]                        = rays[static_cast<std::size_t>(2 * line)];
                                auto const opposite                         = directions{{{df, dr}, {-df, -dr}, {df, dr}, {-df, -dr}}};
                                m_lines[sq][static_cast<std::size_t>(line)] = walked_attacks(sq, 0, opposite);
                        }
                }
        }

        [[nodiscard]] auto operator()(int sq, bitboard occupied) const noexcept
                -> bitboard
        {
                return along(sq, occupied, m_lines[sq][0]) | along(sq, occupied, m_lines[sq][1]);
        }

private:
        [[nodiscard]] static auto along(int sq, bitboard occupied, bitboard line) noexcept
                -> bitboard
        {
                auto const o       = occupied & line;
                auto const s       = bit(sq);
                auto const forward = o - 2 * s;
                auto const reverse = xstd::bit_reverse(xstd::bit_reverse(o) - 2 * xstd::bit_reverse(s));
                return (forward ^ reverse) & line;
        }

        std::array<std::array<bitboard, 2>, 64> m_lines{};
};

} // namespace perft

#endif // XSTD_INTS_EXAMPLES_PERFT_SLIDERS_HPP
