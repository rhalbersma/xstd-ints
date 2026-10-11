//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/detail/bit_limb.hpp> // bit_limb, bit_limb_width
#include <xstd/ints/detail/portable.hpp> // compress_by_rank, expand_by_select, expand_by_walk, select
#include <test/bit_reference.hpp>        // alternating_ones, for_each_edge_value_at, for_each_sweep_pair, for_each_sweep_value, holds_at_every_position, next_pseudo_random, reference_bit_compress, reference_bit_expand
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                         // array
#include <bit>                           // popcount
#include <cstdint>                       // uint64_t
#include <limits>                        // numeric_limits
#include <ranges>                        // iota
#include <utility>                       // make_integer_sequence

namespace {

using xstd::ints::detail::bit_limb;
namespace portable = xstd::ints::detail::portable;

constexpr auto ones = std::numeric_limits<bit_limb>::max();

// The position of x's k-th one, found one bit at a time.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): select's signature, the word and then the rank
constexpr auto reference_select(bit_limb x, unsigned k)
        -> unsigned
{
        for (auto const n : std::views::iota(0U, unsigned{xstd::ints::detail::bit_limb_width})) {
                if (((x >> n) & 1U) != 0U) {
                        if (k == 0U) {
                                return n;
                        }
                        --k;
                }
        }
        return unsigned{xstd::ints::detail::bit_limb_width};
}

constexpr auto kernels_agree(bit_limb x, bit_limb m)
        -> bool
{
        return portable::compress_by_rank(x, m) == test::reference_bit_compress(x, m) and
               portable::expand_by_walk(x, m) == test::reference_bit_expand(x, m) and
               portable::expand_by_select(x, m) == test::reference_bit_expand(x, m);
}

constexpr auto select_agrees(bit_limb x)
        -> bool
{
        auto agrees = true;
        for (auto const k : std::views::iota(0U, static_cast<unsigned>(std::popcount(x)))) {
                agrees = agrees and portable::select(x, k) == reference_select(x, k);
        }
        return agrees;
}

constexpr auto agrees_at(int k)
        -> bool
{
        auto agrees = true;
        // An array, not a braced list: MSVC 19.44 crashes constant-evaluating a loop over the latter.
        for (auto const partner : std::array{ones, test::alternating_ones<bit_limb>()}) {
                test::for_each_edge_value_at<bit_limb>(k, [&](bit_limb edge) -> void {
                        agrees = agrees and kernels_agree(partner, edge) and kernels_agree(edge, partner) and select_agrees(edge);
                });
        }
        return agrees;
}

// Few selected bits over a wide mask, the shape of an endgame index: the case the select kernel is for.
template<class F>
auto for_each_sparse_pair(F f)
        -> void
{
        auto state = std::uint64_t{0x9e3779b97f4a7c15U};
        for ([[maybe_unused]] auto const sample : std::views::iota(0UZ, 4096UZ)) {
                auto const dense  = test::next_pseudo_random<bit_limb>(state);
                auto const mask   = dense | test::next_pseudo_random<bit_limb>(state);
                auto const half   = test::next_pseudo_random<bit_limb>(state);
                auto const sparse = half & test::next_pseudo_random<bit_limb>(state);
                auto const source = sparse & test::next_pseudo_random<bit_limb>(state);
                f(source, mask);
                f(source & 0x1FU, mask);
        }
}

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Detail)
BOOST_AUTO_TEST_SUITE(Portable)

// Every kernel against its bit-at-a-time reference, edge values at every position, at compile time.
BOOST_AUTO_TEST_CASE(KernelsAgreeWithTheReferenceAtCompileTime)
{
        static_assert(test::holds_at_every_position<agrees_at>(std::make_integer_sequence<int, std::numeric_limits<bit_limb>::digits>{}));
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(KernelsAgreeWithTheReference)
{
        auto mismatches = 0UZ;
        test::for_each_sweep_pair<bit_limb>([&](bit_limb x, bit_limb m) -> void { mismatches += kernels_agree(x, m) ? 0UZ : 1UZ; });
        for_each_sparse_pair([&](bit_limb x, bit_limb m) -> void { mismatches += kernels_agree(x, m) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_CASE(SelectAgreesWithTheReference)
{
        auto mismatches = 0UZ;
        test::for_each_sweep_value<bit_limb>([&](bit_limb x) -> void { mismatches += select_agrees(x) ? 0UZ : 1UZ; });
        BOOST_CHECK_EQUAL(mismatches, 0UZ);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
