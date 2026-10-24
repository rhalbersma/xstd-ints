//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TYPES_HPP
#define XSTD_TYPES_HPP

#include <xstd/ints.hpp>                  // the functions under test
#include <xstd/ints/ext/absl/int128.hpp>  // absl::int128, absl::uint128
#include <xstd/ints/ext/boost/int128.hpp> // boost::int128::int128, boost::int128::uint128

// The bit-precise widths xstd's own kernels take, either side of each byte and word boundary, F applied to each.
#ifdef __BITINT_MAXWIDTH__
#define XSTD_TYPES_BIT_UINT(F) \
        F(unsigned _BitInt(8)) \
        F(unsigned _BitInt(9)) \
        F(unsigned _BitInt(17)) \
        F(unsigned _BitInt(33)) \
        F(unsigned _BitInt(65)) \
        F(unsigned _BitInt(127)) \
        F(unsigned _BitInt(128)) \
        F(unsigned _BitInt(129)) \
        F(unsigned _BitInt(256))
#define XSTD_TYPES_BIT_INT(F) \
        F(signed _BitInt(8)) \
        F(signed _BitInt(9)) \
        F(signed _BitInt(17)) \
        F(signed _BitInt(33)) \
        F(signed _BitInt(65)) \
        F(signed _BitInt(127)) \
        F(signed _BitInt(128)) \
        F(signed _BitInt(129)) \
        F(signed _BitInt(256))
#else
#define XSTD_TYPES_BIT_UINT(F)
#define XSTD_TYPES_BIT_INT(F)
#endif

// The unsigned types a constant expression can hold, and those a run can, absl::uint128 having no constexpr operators.
#define XSTD_TYPES_UNSIGNED_CONSTEXPR(F) XSTD_TYPES_BIT_UINT(F) F(boost::int128::uint128)
#define XSTD_TYPES_UNSIGNED(F) XSTD_TYPES_UNSIGNED_CONSTEXPR(F) F(absl::uint128)
#define XSTD_TYPES_SIGNED(F) XSTD_TYPES_BIT_INT(F) F(boost::int128::int128) F(absl::int128)

// The statements a test's own instantiation list makes, for one more type.
#define XSTD_TYPES_STATIC_TEST(T) static_assert(test<T>());
#define XSTD_TYPES_TEST(T) test<T>();
#define XSTD_TYPES_TEST_ABS(T) test_abs<T, T>();

#endif // XSTD_TYPES_HPP
