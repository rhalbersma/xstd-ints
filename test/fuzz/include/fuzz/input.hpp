//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef FUZZ_INPUT_HPP
#define FUZZ_INPUT_HPP

#include <xstd/ints/cstdint.hpp>                   // uint128
#include <xstd/ints/limits.hpp>                    // numeric_limits
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <test/bit/shift_count.hpp>                // shift_count_t
#include <test/bit/sweep.hpp>                      // bit_precise_sweep_types
#include <cstddef>                                 // size_t
#include <cstdint>                                 // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstdlib>                                 // abort
#include <ranges>                                  // iota
#include <span>                                    // span
#include <tuple>                                   // tuple, tuple_cat
#include <type_traits>                             // conditional_t
#include <utility>                                 // declval

namespace fuzz {

// Every width the public functions take: one block, part of one, two, and the bit-precise ones across block boundaries.
using unsigned_types = decltype(std::tuple_cat(
        std::declval<std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, xstd::uint128>>(),
        std::declval<test::bit_precise_sweep_types>()
));

// The signed counterparts above two bits: the oracles halve by dividing by 2, which a signed _BitInt(2) cannot hold.
template<class... T>
[[nodiscard]] auto make_signed_each(std::tuple<T...>) -> decltype(std::tuple_cat(std::declval<std::conditional_t<(xstd::numeric_limits<T>::digits > 2), std::tuple<xstd::make_signed_t<T>>, std::tuple<>>>()...));

using signed_types = decltype(make_signed_each(std::declval<unsigned_types>()));

using integer_types = decltype(std::tuple_cat(std::declval<signed_types>(), std::declval<unsigned_types>()));

template<class T>
inline constexpr auto byte_width = (static_cast<std::size_t>(xstd::numeric_limits<xstd::make_unsigned_t<T>>::digits) + 7UZ) / 8UZ;

[[nodiscard]] inline auto byte_at(std::span<std::uint8_t const> input, std::size_t offset)
        -> std::uint8_t
{
        return offset < input.size() ? input[offset] : std::uint8_t{0};
}

// Little-endian bytes from offset, as many as T is wide, reading zero past the end of the input.
template<class T>
[[nodiscard]] auto read_at(std::span<std::uint8_t const> input, std::size_t offset)
        -> T
{
        using U    = xstd::make_unsigned_t<T>;
        auto value = U{0};
        // Each byte shifted into place rather than the value shifted past it: a _BitInt(2) has no room to shift by 8.
        for (auto const k : std::views::iota(0UZ, byte_width<T>)) {
                value = static_cast<U>(value | static_cast<U>(static_cast<U>(byte_at(input, offset + k)) << static_cast<test::shift_count_t<U>>(8UZ * k)));
        }
        return static_cast<T>(value);
}

// An anchor byte and an offset as wide as the widest type, so the same bytes name a value at every width.
inline constexpr auto operand_size = 1UZ + 32UZ;

// The k-th operand: zero, the minimum or the maximum, plus a wrapping offset, so one seed sits at every width's edge.
template<class T>
[[nodiscard]] auto operand(std::span<std::uint8_t const> input, std::size_t k)
        -> T
{
        using U           = xstd::make_unsigned_t<T>;
        auto const at     = k * operand_size;
        auto const anchor = byte_at(input, at) % 3U;
        auto const base   = anchor == 0U ? T{0} : anchor == 1U ? xstd::numeric_limits<T>::min()
                                                               : xstd::numeric_limits<T>::max();
        return static_cast<T>(static_cast<U>(static_cast<U>(base) + read_at<U>(input, at + 1UZ)));
}

// The k-th byte after the operands, which a target reads as a base, a length or an exponent.
[[nodiscard]] inline auto trailer_at(std::span<std::uint8_t const> input, std::size_t operands, std::size_t k)
        -> std::uint8_t
{
        return byte_at(input, operands * operand_size + k);
}

inline auto check(bool holds)
        -> void
{
        if (not holds) {
                std::abort();
        }
}

} // namespace fuzz

#endif // FUZZ_INPUT_HPP
