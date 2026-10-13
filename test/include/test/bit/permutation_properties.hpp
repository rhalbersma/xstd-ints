//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_BIT_PERMUTATION_PROPERTIES_HPP
#define TEST_BIT_PERMUTATION_PROPERTIES_HPP

#include <xstd/ints/bit/bit_compress.hpp>                    // bit_compress
#include <xstd/ints/bit/bit_expand.hpp>                      // bit_expand
#include <xstd/ints/bit/detail/intrinsic/block_compress.hpp> // IWYU pragma: keep; block_compress, where PEXT is fast
#include <xstd/ints/bit/detail/intrinsic/block_expand.hpp>   // IWYU pragma: keep; block_expand, where PDEP is fast
#include <xstd/ints/bit/detail/intrinsic/bmi2.hpp>           // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/bit/detail/portable/block_compress.hpp>  // block_compress_by_rank
#include <xstd/ints/bit/detail/portable/block_expand.hpp>    // block_expand_by_select, block_expand_by_walk
#include <xstd/ints/bit/popcount.hpp>                        // popcount
#include <xstd/ints/limits.hpp>                              // numeric_limits
#include <test/bit/oracle.hpp>                               // oracle_bit_compress, oracle_bit_expand
#include <test/bit/shift_count.hpp>                          // shift_count_t
#include <cstddef>                                           // size_t

// What compression and expansion owe each other and their oracles, shared by the unit tests and the fuzzer.
namespace test {

// The k lowest bits, k being anything from none to the whole width.
template<class T>
[[nodiscard]] constexpr auto low_ones(int k)
        -> T
{
        return k < xstd::numeric_limits<T>::digits ? static_cast<T>(static_cast<T>(T{1} << static_cast<shift_count_t<T>>(k)) - T{1}) : xstd::numeric_limits<T>::max();
}

// Expanding a compression keeps src's masked bits; compressing an expansion keeps src's popcount(mask) low bits.
template<class T, class Compress, class Expand>
[[nodiscard]] constexpr auto round_trips(T src, T mask, Compress compress, Expand expand)
        -> bool
{
        return expand(compress(src, mask), mask) == static_cast<T>(src & mask) and
               compress(expand(src, mask), mask) == static_cast<T>(src & low_ones<T>(xstd::popcount(mask)));
}

// Both portable expansions agree, and each round-trips with the portable compression.
[[nodiscard]] constexpr auto portable_kernels_round_trip(std::size_t src, std::size_t mask)
        -> bool
{
        namespace portable = xstd::ints::detail::portable;
        return portable::block_expand_by_walk(src, mask) == portable::block_expand_by_select(src, mask) and
               round_trips(src, mask, portable::block_compress_by_rank, portable::block_expand_by_walk) and
               round_trips(src, mask, portable::block_compress_by_rank, portable::block_expand_by_select);
}

#ifdef XSTD_INTS_HAS_FAST_BMI2

// One of PEXT's and PDEP's overloads against the portable kernels at its own width, and round-tripping on its own.
template<class U>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): PEXT's signature, the source and then the mask
[[nodiscard]] inline auto intrinsics_agree_at(std::size_t src, std::size_t mask)
        -> bool
{
        namespace intrinsic = xstd::ints::detail::intrinsic;
        namespace portable  = xstd::ints::detail::portable;
        auto const s        = static_cast<U>(src);
        auto const m        = static_cast<U>(mask);
        auto const compress = [](U x, U y) -> U { return intrinsic::block_compress(x, y); };
        auto const expand   = [](U x, U y) -> U { return intrinsic::block_expand(x, y); };
        return compress(s, m) == portable::block_compress_by_rank(s, m) and expand(s, m) == portable::block_expand_by_walk(s, m) and
               round_trips(s, m, compress, expand);
}

// Every overload, the 32-bit one included, which no block of a 64-bit target reaches through the dispatch.
[[nodiscard]] inline auto intrinsics_agree(std::size_t src, std::size_t mask)
        -> bool
{
        return intrinsics_agree_at<unsigned int>(src, mask) and intrinsics_agree_at<unsigned long>(src, mask) and
               intrinsics_agree_at<unsigned long long>(src, mask);
}

#endif

// The public functions agree with their oracles and round-trip, across every block of T.
template<class T>
[[nodiscard]] constexpr auto permutations_hold(T src, T mask)
        -> bool
{
        auto const compress = [](T x, T y) -> T { return xstd::bit_compress(x, y); };
        auto const expand   = [](T x, T y) -> T { return xstd::bit_expand(x, y); };
        return compress(src, mask) == oracle_bit_compress(src, mask) and expand(src, mask) == oracle_bit_expand(src, mask) and
               round_trips(src, mask, compress, expand);
}

} // namespace test

#endif // TEST_BIT_PERMUTATION_PROPERTIES_HPP
