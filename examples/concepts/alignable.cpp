//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/alignable.hpp> // alignable, nothrow_alignable
#include <xstd/ints/cstdint/int128.hpp>     // int128, uint128, XSTD_HAS_INT128
#include <cstddef>                          // ptrdiff_t, size_t
#include <cstdint>                          // int32_t, uint8_t, uint32_t, uint64_t
#include <iostream>                         // cout

static_assert(xstd::alignable<std::size_t>);
static_assert(xstd::alignable<std::uint8_t const>);
static_assert(xstd::alignable<char8_t>);
static_assert(not xstd::alignable<std::int32_t>);
static_assert(not xstd::alignable<std::ptrdiff_t>);
static_assert(not xstd::alignable<bool>);
static_assert(not xstd::alignable<std::uint8_t*>);
static_assert(xstd::nothrow_alignable<std::uint64_t>);

#ifdef XSTD_HAS_INT128
static_assert(xstd::alignable<xstd::uint128>);
static_assert(not xstd::alignable<xstd::int128>);
#endif

// The bytes to skip from offset to the next multiple of a power-of-two alignment, in the caller's own unsigned type.
template<xstd::alignable T>
[[nodiscard]] constexpr auto padding(T offset, std::size_t alignment) noexcept(xstd::nothrow_alignable<T>)
        -> T
{
        auto const mask = static_cast<T>(alignment - 1);
        auto const zero = static_cast<T>(0UZ);
        return static_cast<T>(static_cast<T>(zero - offset) & mask);
}

static_assert(noexcept(padding(std::uint32_t{13}, 8)));

auto main() -> int
{
        std::cout << "padding(13, 8)    = " << padding(std::uint32_t{13}, 8) << '\n';
        std::cout << "padding(250, 16)  = " << static_cast<unsigned>(padding(std::uint8_t{250}, 16)) << '\n';
        std::cout << "padding(4096, 64) = " << padding(std::uint64_t{4096}, 64) << '\n';
}
