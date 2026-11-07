//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdint/int128.hpp>                   // int128, uint128, XSTD_HAS_INT128
#include <cstdint>                                        // int32_t, int64_t, uint64_t
#include <iostream>                                       // cout
#include <limits>                                         // numeric_limits

static_assert(xstd::nothrow_const_operators<int>);
static_assert(xstd::nothrow_const_operators<std::uint64_t const>);
static_assert(not xstd::nothrow_const_operators<char32_t>);
static_assert(not xstd::nothrow_const_operators<bool>);

#ifdef XSTD_HAS_INT128
static_assert(xstd::nothrow_const_operators<xstd::int128>);
static_assert(xstd::nothrow_const_operators<xstd::uint128>);
#endif

// The average rounded down, without overflow, and noexcept exactly where the operators it calls are.
template<xstd::integer T>
[[nodiscard]] constexpr auto midpoint_floor(T a, T b) noexcept(xstd::nothrow_const_operators<T>)
        -> T
{
        return static_cast<T>((a & b) + ((a ^ b) >> 1UZ));
}

static_assert(noexcept(midpoint_floor(1, 2)));

auto main() -> int
{
        auto const max32 = std::numeric_limits<std::int32_t>::max();
        std::cout << midpoint_floor(max32, max32 - 2) << '\n';
        std::cout << midpoint_floor(std::int64_t{-7}, std::int64_t{2}) << '\n';
        std::cout << midpoint_floor(std::numeric_limits<std::uint64_t>::max(), std::uint64_t{1}) << '\n';
}
