//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BENCH_SUCCINCT_SELECT_DICTIONARY_HPP
#define XSTD_INTS_BENCH_SUCCINCT_SELECT_DICTIONARY_HPP

#include <xstd/ints/bit/bit_select.hpp> // bit_select
#include <xstd/ints/bit/popcount.hpp>   // popcount
#include <algorithm>                    // upper_bound
#include <cstddef>                      // size_t
#include <cstdint>                      // uint64_t
#include <span>                         // span
#include <vector>                       // vector

// A rank/select dictionary as a user of xstd would write one: counts per block, sampled positions, then bit_select.
namespace succinct {

inline constexpr auto words_per_block = std::size_t{8};
inline constexpr auto ones_per_sample = std::uint64_t{8192};

class select_dictionary
{
public:
        explicit select_dictionary(std::span<std::uint64_t const> words)
                : m_words(words)
        {
                auto const blocks = (words.size() + words_per_block - 1) / words_per_block;
                m_counts.reserve(blocks + 1);
                auto ones = std::uint64_t{0};
                for (auto block = std::size_t{0}; block < blocks; ++block) {
                        m_counts.push_back(ones);
                        auto const block_ones = ones_in(block);
                        for (auto next = m_samples.size() * ones_per_sample; next < ones + block_ones; next += ones_per_sample) {
                                m_samples.push_back(block);
                        }
                        ones += block_ones;
                }
                m_counts.push_back(ones);
                m_samples.push_back(blocks == 0 ? 0 : blocks - 1);
        }

        [[nodiscard]] auto ones() const noexcept
                -> std::uint64_t
        {
                return m_counts.back();
        }

        // The number of ones before position pos, which must be less than 64 times the number of words.
        [[nodiscard]] auto rank(std::uint64_t pos) const noexcept
                -> std::uint64_t
        {
                auto const word = static_cast<std::size_t>(pos / 64);
                auto ones       = m_counts[word / words_per_block];
                for (auto w = word / words_per_block * words_per_block; w < word; ++w) {
                        ones += static_cast<std::uint64_t>(xstd::popcount(m_words[w]));
                }
                return ones + static_cast<std::uint64_t>(xstd::popcount(m_words[word] & ((std::uint64_t{1} << (pos % 64)) - 1)));
        }

        // The position of the one of rank k: its sample bounds a binary search over block counts, then a word scan.
        template<class InWord>
        [[nodiscard]] auto select(std::uint64_t k, InWord in_word) const noexcept
                -> std::uint64_t
        {
                auto const sample = static_cast<std::size_t>(k / ones_per_sample);
                auto const first  = m_counts.begin() + static_cast<std::ptrdiff_t>(m_samples[sample]);
                auto const last   = m_counts.begin() + static_cast<std::ptrdiff_t>(m_samples[sample + 1]) + 1;
                auto const block  = static_cast<std::size_t>(std::upper_bound(first, last, k) - m_counts.begin()) - 1;
                auto rank         = k - m_counts[block];
                auto word         = block * words_per_block;
                for (auto ones_here = static_cast<std::uint64_t>(xstd::popcount(m_words[word])); rank >= ones_here; ones_here = static_cast<std::uint64_t>(xstd::popcount(m_words[word]))) {
                        rank -= ones_here;
                        ++word;
                }
                return 64 * word + in_word(m_words[word], rank);
        }

        [[nodiscard]] auto select(std::uint64_t k) const noexcept
                -> std::uint64_t
        {
                return select(k, [](std::uint64_t w, std::uint64_t r) noexcept -> std::uint64_t { return static_cast<std::uint64_t>(xstd::bit_select(w, static_cast<int>(r))); });
        }

private:
        [[nodiscard]] auto ones_in(std::size_t block) const noexcept
                -> std::uint64_t
        {
                auto ones = std::uint64_t{0};
                for (auto w = block * words_per_block; w < std::min(m_words.size(), (block + 1) * words_per_block); ++w) {
                        ones += static_cast<std::uint64_t>(xstd::popcount(m_words[w]));
                }
                return ones;
        }

        std::span<std::uint64_t const> m_words;
        std::vector<std::uint64_t> m_counts;
        std::vector<std::size_t> m_samples;
};

} // namespace succinct

#endif // XSTD_INTS_BENCH_SUCCINCT_SELECT_DICTIONARY_HPP
