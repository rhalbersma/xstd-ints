//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/bit_int.hpp>  // XSTD_HAS_BIT_INT, bit_int, bit_uint
#include <xstd/ints/cstdint/int128.hpp>   // XSTD_HAS_INT128, uint128
#include <xstd/ints/ext/boost/hash2.hpp>  // hash_append, hash_append_int, little_endian_flavor
#include <xstd/ints/ext/boost/int128.hpp> // int128, uint128
#include <boost/hash2/fnv1a.hpp>          // fnv1a_64
#include <cstdint>                        // int32_t, int64_t, uint64_t
#include <iomanip>                        // hex, setfill, setw
#include <iostream>                       // cout

namespace {

template<class T>
[[nodiscard]] constexpr auto digest(T value)
        -> std::uint64_t
{
        auto hash = boost::hash2::fnv1a_64{};
        xstd::hash_append_int(hash, boost::hash2::little_endian_flavor{}, value);
        return hash.result();
}

[[nodiscard]] constexpr auto digest_of_words(std::uint64_t low, std::uint64_t high)
        -> std::uint64_t
{
        auto hash = boost::hash2::fnv1a_64{};
        boost::hash2::hash_append(hash, boost::hash2::little_endian_flavor{}, low);
        boost::hash2::hash_append(hash, boost::hash2::little_endian_flavor{}, high);
        return hash.result();
}

auto print_digest(char const* name, std::uint64_t value)
        -> void
{
        std::cout << name << std::hex << std::setfill('0') << std::setw(16) << value << '\n';
}

} // namespace

// Every 128-bit type hashes as every other, and a bit-precise type as the narrowest standard width that holds it.
#ifdef XSTD_HAS_INT128
static_assert(digest(xstd::uint128{42}) == digest(boost::int128::uint128{42}));
#endif
#ifdef XSTD_HAS_BIT_INT
static_assert(digest(xstd::bit_uint<128>{42}) == digest(boost::int128::uint128{42}));
static_assert(digest(xstd::bit_int<24>{-42}) == digest(std::int32_t{-42}));
#endif

auto main() -> int
{
        print_digest("std::int32_t{-42}:          ", digest(std::int32_t{-42}));
        print_digest("std::int64_t{-42}:          ", digest(std::int64_t{-42}));
        print_digest("boost::int128::int128{-42}: ", digest(boost::int128::int128{-42}));
        print_digest("its words, low first:       ", digest_of_words(static_cast<std::uint64_t>(-42), ~std::uint64_t{0}));
        print_digest("boost::int128::uint128{42}: ", digest(boost::int128::uint128{42}));
        print_digest("its words, low first:       ", digest_of_words(42, 0));
}
