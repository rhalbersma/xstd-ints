//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_EXACT_WIDTH_TYPES_HPP
#define TEST_EXACT_WIDTH_TYPES_HPP

#include <xstd/ints/cstdint.hpp> // XSTD_HAS_BIT_INT, bit_int, bit_uint, int128, uint128
#include <cstdint>               // exact-width integer types
#include <tuple>                 // tuple, tuple_cat
#include <utility>               // declval

// The optional pairs reached the way any consumer reaches them: the probe here, the associations in the adapter.
#if __has_include(<boost/int128.hpp>)
#define TEST_HAS_BOOST_INT128
#include <xstd/ints/ext/boost/int128.hpp> // int128, uint128
#endif

#if __has_include(<absl/numeric/int128.h>)
#define TEST_HAS_ABSL_INT128
#include <xstd/ints/ext/absl/int128.hpp> // int128, uint128
#endif

// The exact-width lists, one per library, so a case can name just the widths it can use.
namespace test {

// The widths the standard names, 8 through 64 bits.
using std_signed_types = std::tuple<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;
using std_unsigned_types = std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

// And the width xstd names on top of those.
using xstd_signed_types = std::tuple<xstd::int128>;
using xstd_unsigned_types = std::tuple<xstd::uint128>;

// And a 128-bit type from outside the library, when the build has one.
#ifdef TEST_HAS_BOOST_INT128
using boost_signed_types = std::tuple<boost::int128::int128>;
using boost_unsigned_types = std::tuple<boost::int128::uint128>;
#else
using boost_signed_types = std::tuple<>;
using boost_unsigned_types = std::tuple<>;
#endif

// A second, conditional on an intrinsic: without one its operator/ is not constexpr.
#ifdef TEST_HAS_ABSL_INT128
using absl_signed_types = std::tuple<absl::int128>;
using absl_unsigned_types = std::tuple<absl::uint128>;
#else
using absl_signed_types = std::tuple<>;
using absl_unsigned_types = std::tuple<>;
#endif

// Abseil falls back to non-constexpr division where the compiler has no intrinsic 128-bit type.
template<class T>
inline constexpr bool has_constexpr_division = true;

#if defined(TEST_HAS_ABSL_INT128) and not defined(ABSL_HAVE_INTRINSIC_INT128)
template<>
inline constexpr bool has_constexpr_division<absl::int128> = false;
template<>
inline constexpr bool has_constexpr_division<absl::uint128> = false;
#endif

// Native bit-precise widths, from the two bits current Clang's signed type needs up.
#ifdef XSTD_HAS_BIT_INT

// How wide the lists may go, a property of the build: dividing past 64 bits calls compiler-rt, unlinked by clang-cl.
#ifdef _MSC_VER
#define TEST_BIT_PRECISE_MAX 64
#elif __BITINT_MAXWIDTH__ >= 256
#define TEST_BIT_PRECISE_MAX 256
#else
#define TEST_BIT_PRECISE_MAX 128
#endif

// The widths every implementation of the extension divides without help from a runtime.
using narrow_bit_precise_signed_types =
        std::tuple<xstd::bit_int<8>, xstd::bit_int<16>, xstd::bit_int<32>, xstd::bit_int<64>>;
using narrow_bit_precise_unsigned_types =
        std::tuple<xstd::bit_uint<8>, xstd::bit_uint<16>, xstd::bit_uint<32>, xstd::bit_uint<64>>;

// And the two above them, each asking the ceiling rather than assuming one.
#if TEST_BIT_PRECISE_MAX >= 128
using wide_bit_precise_signed_types = std::tuple<xstd::bit_int<128>>;
using wide_bit_precise_unsigned_types = std::tuple<xstd::bit_uint<128>>;
#else
using wide_bit_precise_signed_types = std::tuple<>;
using wide_bit_precise_unsigned_types = std::tuple<>;
#endif

#if TEST_BIT_PRECISE_MAX >= 256
using widest_bit_precise_signed_types = std::tuple<xstd::bit_int<256>>;
using widest_bit_precise_unsigned_types = std::tuple<xstd::bit_uint<256>>;
#else
using widest_bit_precise_signed_types = std::tuple<>;
using widest_bit_precise_unsigned_types = std::tuple<>;
#endif

using bit_precise_signed_types = decltype(std::tuple_cat(
        std::declval<narrow_bit_precise_signed_types>(), std::declval<wide_bit_precise_signed_types>(),
        std::declval<widest_bit_precise_signed_types>()
));
using bit_precise_unsigned_types = decltype(std::tuple_cat(
        std::declval<narrow_bit_precise_unsigned_types>(), std::declval<wide_bit_precise_unsigned_types>(),
        std::declval<widest_bit_precise_unsigned_types>()
));
#else
using bit_precise_signed_types = std::tuple<>;
using bit_precise_unsigned_types = std::tuple<>;
#endif

// Constant-evaluation tests omit only Abseil's fallback implementation.
#if defined(TEST_HAS_ABSL_INT128) and not defined(ABSL_HAVE_INTRINSIC_INT128)
using constexpr_absl_signed_types = std::tuple<>;
using constexpr_absl_unsigned_types = std::tuple<>;
#else
using constexpr_absl_signed_types = absl_signed_types;
using constexpr_absl_unsigned_types = absl_unsigned_types;
#endif
using constexpr_exact_width_signed_integer_types = decltype(std::tuple_cat(
        std::declval<std_signed_types>(), std::declval<xstd_signed_types>(), std::declval<boost_signed_types>(),
        std::declval<constexpr_absl_signed_types>(), std::declval<bit_precise_signed_types>()
));
using constexpr_exact_width_unsigned_integer_types = decltype(std::tuple_cat(
        std::declval<std_unsigned_types>(), std::declval<xstd_unsigned_types>(), std::declval<boost_unsigned_types>(),
        std::declval<constexpr_absl_unsigned_types>(), std::declval<bit_precise_unsigned_types>()
));

// The authoritative configured test universe; subsets remain for contracts that differ by sign.
using exact_width_signed_integer_types = decltype(std::tuple_cat(
        std::declval<std_signed_types>(), std::declval<xstd_signed_types>(), std::declval<boost_signed_types>(),
        std::declval<absl_signed_types>(), std::declval<bit_precise_signed_types>()
));
using exact_width_unsigned_integer_types = decltype(std::tuple_cat(
        std::declval<std_unsigned_types>(), std::declval<xstd_unsigned_types>(), std::declval<boost_unsigned_types>(),
        std::declval<absl_unsigned_types>(), std::declval<bit_precise_unsigned_types>()
));
using exact_width_integer_types = decltype(std::tuple_cat(
        std::declval<exact_width_signed_integer_types>(),
        std::declval<exact_width_unsigned_integer_types>()
));

} // namespace test

#endif // TEST_EXACT_WIDTH_TYPES_HPP
