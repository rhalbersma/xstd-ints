//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/bit_mask.hpp> // bit_mask
#include <bitset>                          // bitset
#include <charconv>                        // chars_format
#include <cstddef>                         // byte
#include <cstdint>                         // int32_t, uint8_t, uint16_t
#include <filesystem>                      // file_type, perms
#include <iostream>                        // boolalpha, cout

static_assert(xstd::bit_mask<std::uint16_t>);
static_assert(xstd::bit_mask<std::bitset<8> const>);
static_assert(xstd::bit_mask<std::filesystem::perms>);
static_assert(xstd::bit_mask<std::chars_format>);
static_assert(xstd::bit_mask<std::byte>);

// An int's ~ is negative, bool and char are no bit sets, and file_type enumerates rather than combines.
static_assert(not xstd::bit_mask<std::int32_t>);
static_assert(not xstd::bit_mask<bool>);
static_assert(not xstd::bit_mask<char>);
static_assert(not xstd::bit_mask<std::filesystem::file_type>);

// Whether every flag of wanted is set in flags, for an unsigned integer, a bitset or a bitmask enumeration alike.
template<xstd::bit_mask T>
[[nodiscard]] auto all_set(T flags, T wanted)
        -> bool
{
        return static_cast<T>(flags & wanted) == wanted;
}

auto main() -> int
{
        using std::filesystem::perms;
        std::cout << std::boolalpha;
        std::cout << all_set(perms::owner_all, perms::owner_read | perms::owner_write) << '\n';
        std::cout << all_set(perms::owner_read, perms::owner_read | perms::owner_exec) << '\n';
        std::cout << all_set(std::uint8_t{0b1110}, std::uint8_t{0b0110}) << '\n';
        std::cout << all_set(std::bitset<8>{"1010"}, std::bitset<8>{"0011"}) << '\n';
}
