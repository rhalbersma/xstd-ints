//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit.hpp>                                // bit_ceil, bit_compress, bit_expand, bit_floor, bit_repeat, bit_reverse, bit_select, bit_width, byteswap, countl_one, countl_zero, countr_one, countr_zero, has_single_bit, popcount, rotl, rotr
#include <xstd/ints/bit/detail/portable/block_compress.hpp> // block_compress_by_rank
#include <xstd/ints/bit/detail/portable/block_expand.hpp>   // block_expand_by_select, block_expand_by_walk
#include <xstd/ints/bit/detail/portable/block_select.hpp>   // block_select_by_broadword
#include <xstd/ints/cstdint.hpp>                            // XSTD_HAS_BIT_INT, bit_uint
#include <sweep.hpp>                                        // for_each_row, mix, passed, patterns
#include <atomic>                                           // atomic
#include <cstddef>                                          // size_t
#include <cstdint>                                          // uint16_t, uint64_t
#include <iostream>                                         // cout

#if __has_include(<simde/x86/bmi.h>)
#define DIFFERENTIAL_HAS_SIMDE
#include <simde/simde-common.h> // SIMDE_FUNCTION_ATTRIBUTES, which bmi.h uses without including
#include <simde/x86/bmi.h>      // simde_pdep_u64, simde_pext_u64
#endif

#if __has_include(<sux/support/common.hpp>)
#define DIFFERENTIAL_HAS_SUX
#include <sux/support/common.hpp> // select64
#endif

#if defined(XSTD_HAS_BIT_INT) and __has_include(<boost/multiprecision/cpp_int.hpp>)
#if __BITINT_MAXWIDTH__ >= 1024
#define DIFFERENTIAL_HAS_MULTIPRECISION
#include <boost/multiprecision/cpp_int.hpp> // cpp_int_backend, export_bits, import_bits, lsb, msb, number
#include <algorithm>                        // reverse
#include <vector>                           // vector
#endif
#endif

namespace {

using exhaustive::mix;

// Pseudo-random words of every density: a draw, and its AND and OR with a second and a third.
[[nodiscard]] auto word(std::uint64_t seed)
        -> std::uint64_t
{
        auto const a = mix(seed);
        auto const b = mix(a);
        auto const c = mix(b);
        switch (c % 5U) {
                case 0:
                        return a & b & c;
                case 1:
                        return a & b;
                case 2:
                        return a;
                case 3:
                        return a | b;
                default:
                        return a | b | c;
        }
}

inline constexpr auto random_pairs = std::uint64_t{1} << 26U;

#ifdef DIFFERENTIAL_HAS_SIMDE
// SIMDe's portable PEXT and PDEP, built with SIMDE_NO_NATIVE so that neither becomes the instruction it stands for.
[[nodiscard]] auto agrees_with_simde()
        -> bool
{
        namespace portable = xstd::ints::detail::portable;
        auto failures      = std::atomic<std::uint64_t>{0};
        auto const check   = [&](std::uint64_t src, std::uint64_t mask) -> void {
                auto const s = static_cast<std::size_t>(src);
                auto const m = static_cast<std::size_t>(mask);
                auto const x = simde_pext_u64(src, mask);
                auto const d = simde_pdep_u64(src, mask);
                if (portable::block_compress_by_rank(s, m) != x or portable::block_expand_by_walk(s, m) != d or portable::block_expand_by_select(s, m) != d) {
                        ++failures;
                }
        };
        exhaustive::for_each_row([&](std::uint16_t mask) -> void {
                for (auto src = 0U; src < exhaustive::patterns; ++src) {
                        check(src, mask);
                }
        });
        exhaustive::for_each_row([&](std::uint16_t row) -> void {
                for (auto i = std::uint64_t{0}; i < random_pairs / exhaustive::patterns; ++i) {
                        auto const seed = (std::uint64_t{row} << 32U) | i;
                        check(word(seed), word(~seed));
                }
        });
        return exhaustive::passed("SIMDe's PEXT and PDEP", failures);
}
#endif

#ifdef DIFFERENTIAL_HAS_SUX
// Vigna's select64 at every rank of every 16-bit word, low and high in the block, then of pseudo-random words.
[[nodiscard]] auto agrees_with_sux()
        -> bool
{
        auto failures    = std::atomic<std::uint64_t>{0};
        auto const check = [&](std::uint64_t x) -> void {
                for (auto k = 0U; k < static_cast<unsigned>(xstd::popcount(x)); ++k) {
                        if (xstd::ints::detail::portable::block_select_by_broadword(static_cast<std::size_t>(x), k) != sux::select64(x, k)) {
                                ++failures;
                        }
                }
        };
        exhaustive::for_each_row([&](std::uint16_t x) -> void {
                check(x);
                check(std::uint64_t{x} << 48U);
                for (auto i = std::uint64_t{0}; i < 64; ++i) {
                        check(word((std::uint64_t{x} << 8U) | i));
                }
        });
        return exhaustive::passed("sux's select64", failures);
}
#endif

#ifdef DIFFERENTIAL_HAS_MULTIPRECISION
// Fixed-width unsigned arithmetic that wraps modulo 2^N, from a library that shares no code with xstd.
template<std::size_t N>
using reference = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<N, N, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;

template<std::size_t N>
[[nodiscard]] auto to_reference(xstd::bit_uint<N> x)
        -> reference<N>
{
        auto r = reference<N>{0};
        for (auto limb = std::size_t{0}; limb * 64 < N; ++limb) {
                r |= reference<N>{static_cast<std::uint64_t>(x >> (64 * limb))} << (64 * limb);
        }
        return r;
}

template<std::size_t N>
[[nodiscard]] auto from_reference(reference<N> const& r)
        -> xstd::bit_uint<N>
{
        auto x = xstd::bit_uint<N>{0};
        for (auto limb = std::size_t{0}; limb * 64 < N; ++limb) {
                auto const bits = static_cast<std::uint64_t>((r >> (64 * limb)) & reference<N>{~std::uint64_t{0}});
                x |= static_cast<xstd::bit_uint<N>>(static_cast<xstd::bit_uint<N>>(bits) << (64 * limb));
        }
        return x;
}

template<std::size_t N>
[[nodiscard]] auto value(std::uint64_t seed)
        -> xstd::bit_uint<N>
{
        auto x = xstd::bit_uint<N>{0};
        for (auto limb = std::size_t{0}; limb * 64 < N; ++limb) {
                x |= static_cast<xstd::bit_uint<N>>(static_cast<xstd::bit_uint<N>>(word(seed * 64 + limb)) << (64 * limb));
        }
        return x;
}

template<std::size_t N>
[[nodiscard]] auto ones(reference<N> r)
        -> int
{
        auto n = 0;
        for (; r != 0; r &= r - 1) {
                ++n;
        }
        return n;
}

template<std::size_t N>
[[nodiscard]] auto rotate_left(reference<N> const& r, int s)
        -> reference<N>
{
        auto const n = static_cast<int>(N);
        auto const k = static_cast<unsigned>(((s % n) + n) % n);
        return k == 0 ? r : static_cast<reference<N>>((r << k) | (r >> (N - k)));
}

// Every <bit> function at width N against the reference, on pseudo-random values of every density and on the edges.
template<std::size_t N>
[[nodiscard]] auto agrees_with_multiprecision()
        -> std::uint64_t
{
        using T              = xstd::bit_uint<N>;
        using R              = reference<N>;
        auto failures        = std::uint64_t{0};
        auto const n         = static_cast<int>(N);
        auto const check_one = [&](T x, T y) -> void {
                auto const r    = to_reference(x);
                auto const m    = to_reference(y);
                auto const pop  = ones(r);
                auto const lz   = r == 0 ? n : n - 1 - static_cast<int>(msb(r));
                auto const tz   = r == 0 ? n : static_cast<int>(lsb(r));
                auto const nr   = static_cast<R>(~r);
                auto const lo   = nr == 0 ? n : n - 1 - static_cast<int>(msb(nr));
                auto const to   = nr == 0 ? n : static_cast<int>(lsb(nr));
                auto compressed = R{0};
                auto expanded   = R{0};
                auto reversed   = R{0};
                for (auto i = 0, j = 0; i < n; ++i) {
                        if (bit_test(m, static_cast<unsigned>(i))) {
                                if (bit_test(r, static_cast<unsigned>(i))) {
                                        bit_set(compressed, static_cast<unsigned>(j));
                                }
                                if (bit_test(r, static_cast<unsigned>(j))) {
                                        bit_set(expanded, static_cast<unsigned>(i));
                                }
                                ++j;
                        }
                        if (bit_test(r, static_cast<unsigned>(i))) {
                                bit_set(reversed, static_cast<unsigned>(n - 1 - i));
                        }
                }
                auto ok = xstd::popcount(x) == pop and xstd::countl_zero(x) == lz and xstd::countr_zero(x) == tz and xstd::countl_one(x) == lo and xstd::countr_one(x) == to;
                ok      = ok and xstd::bit_width(x) == (r == 0 ? 0 : static_cast<int>(msb(r)) + 1) and xstd::has_single_bit(x) == (pop == 1);
                ok      = ok and to_reference(xstd::bit_floor(x)) == (r == 0 ? R{0} : static_cast<R>(R{1} << msb(r)));
                if (r <= 1) {
                        ok = ok and xstd::bit_ceil(x) == T{1};
                } else if (static_cast<int>(msb(static_cast<R>(r - 1))) + 1 < n) {
                        ok = ok and to_reference(xstd::bit_ceil(x)) == static_cast<R>(R{1} << (msb(static_cast<R>(r - 1)) + 1));
                }
                for (auto const s : {0, 1, 63, 64, n - 1, n, n + 5, -1, -64, -n - 3}) {
                        ok = ok and to_reference(xstd::rotl(x, s)) == rotate_left(r, s) and to_reference(xstd::rotr(x, s)) == rotate_left(r, -s);
                }
                ok = ok and to_reference(xstd::bit_reverse(x)) == reversed;
                ok = ok and to_reference(xstd::bit_compress(x, y)) == compressed and to_reference(xstd::bit_expand(x, y)) == expanded;
                if constexpr (N % 8 == 0) {
                        auto bytes = std::vector<unsigned char>{};
                        export_bits(r, std::back_inserter(bytes), 8, false);
                        bytes.resize(N / 8);
                        std::reverse(bytes.begin(), bytes.end());
                        auto swapped = R{0};
                        import_bits(swapped, bytes.begin(), bytes.end(), 8, false);
                        ok = ok and to_reference(xstd::byteswap(x)) == swapped;
                }
                for (auto i = 0, seen = 0; i < n; ++i) {
                        if (bit_test(r, static_cast<unsigned>(i))) {
                                if (seen == 0 or seen == pop / 2 or seen == pop - 1) {
                                        ok = ok and xstd::bit_select(x, seen) == i;
                                }
                                ++seen;
                        }
                }
                for (auto const l : {1, 3, 64, n / 2, n}) {
                        auto repeated = R{0};
                        for (auto i = 0; i < n; ++i) {
                                if (bit_test(r, static_cast<unsigned>(i % l))) {
                                        bit_set(repeated, static_cast<unsigned>(i));
                                }
                        }
                        ok = ok and to_reference(xstd::bit_repeat(x, l)) == repeated;
                }
                failures += ok ? 0 : 1;
        };
        auto const all = from_reference<N>(static_cast<R>(~R{0}));
        for (auto const edge : {T{0}, T{1}, all, static_cast<T>(all >> 1), static_cast<T>(T{1} << (N - 1)), static_cast<T>(T{1} << 64)}) {
                check_one(edge, all);
                check_one(edge, value<N>(N));
        }
        for (auto i = std::uint64_t{0}; i < 20'000; ++i) {
                check_one(value<N>(2 * i), value<N>(2 * i + 1));
        }
        return failures;
}

[[nodiscard]] auto agree_with_multiprecision()
        -> bool
{
        auto failures = std::uint64_t{0};
        failures += agrees_with_multiprecision<129>();
        failures += agrees_with_multiprecision<192>();
        failures += agrees_with_multiprecision<255>();
        failures += agrees_with_multiprecision<256>();
        failures += agrees_with_multiprecision<384>();
        failures += agrees_with_multiprecision<1024>();
        return exhaustive::passed("Boost.Multiprecision at 129 to 1024 bits", failures);
}
#endif

} // namespace

// Each portable kernel, and <bit> above 128 bits, against an implementation xstd did not write; absent ones skip.
auto main() -> int
{
        auto ok = true;
#ifdef DIFFERENTIAL_HAS_SIMDE
        ok = agrees_with_simde() and ok;
#else
        std::cout << "SIMDe not found: skipped\n";
#endif
#ifdef DIFFERENTIAL_HAS_SUX
        ok = agrees_with_sux() and ok;
#else
        std::cout << "sux not found: skipped\n";
#endif
#ifdef DIFFERENTIAL_HAS_MULTIPRECISION
        ok = agree_with_multiprecision() and ok;
#else
        std::cout << "Boost.Multiprecision or unsigned _BitInt(1024) not available: skipped\n";
#endif
        std::cout << (ok ? "every implementation agrees\n" : "disagreement found\n");
        return ok ? 0 : 1;
}
