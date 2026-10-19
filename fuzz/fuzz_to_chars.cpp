//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/charconv/to_chars.hpp>     // to_chars, to_chars_max_size
#include <xstd/ints/cstdint.hpp>               // bit_uint
#include <xstd/ints/type_traits/is_signed.hpp> // is_signed_v
#include <fuzz/input.hpp>                      // check, integer_types, operand, trailer_at
#include <algorithm>                           // all_of, equal, fill
#include <array>                               // array
#include <cstddef>                             // size_t
#include <cstdint>                             // uint8_t
#include <span>                                // span
#include <string>                              // string
#include <system_error>                        // errc
#include <tuple>                               // apply

namespace {

// Wide enough to hold the magnitude of every fuzzed type's minimum.
using wide = xstd::bit_uint<256>;

// A byte no digit and no sign is, so a write outside the answer shows.
constexpr auto untouched = '#';

// Digits by long division in a type of its own, most significant first, after a sign where the value is negative.
template<class T>
[[nodiscard]] auto oracle_to_chars(T value, int base)
        -> std::string
{
        auto negative = false;
        if constexpr (xstd::is_signed_v<T>) {
                negative = value < T{0};
        }
        auto magnitude = negative ? static_cast<wide>(wide{0} - static_cast<wide>(value)) : static_cast<wide>(value);
        auto reversed  = std::string{};
        do {
                reversed += "0123456789abcdefghijklmnopqrstuvwxyz"[static_cast<std::size_t>(magnitude % static_cast<wide>(base))];
                magnitude = static_cast<wide>(magnitude / static_cast<wide>(base));
        } while (magnitude != 0U);
        if (negative) {
                reversed += '-';
        }
        return {reversed.rbegin(), reversed.rend()};
}

template<class T>
auto check_at(T value, int base, std::size_t length)
        -> void
{
        auto buffer = std::array<char, 256>{};
        std::ranges::fill(buffer, untouched);
        auto* const first    = buffer.data();
        auto* const last     = first + length;
        auto const expected  = oracle_to_chars(value, base);
        auto const [ptr, ec] = xstd::to_chars(first, last, value, base);
        fuzz::check(expected.size() <= xstd::to_chars_max_size<T>);
        if (expected.size() > length) {
                // Too short by any amount: value_too_large, ptr at last, and nothing written past it.
                fuzz::check(ec == std::errc::value_too_large and ptr == last);
                fuzz::check(std::all_of(last, buffer.data() + buffer.size(), [](char c) -> bool { return c == untouched; }));
        } else {
                fuzz::check(ec == std::errc{} and ptr == first + expected.size());
                fuzz::check(std::equal(first, ptr, expected.begin(), expected.end()));
                fuzz::check(std::all_of(ptr, buffer.data() + buffer.size(), [](char c) -> bool { return c == untouched; }));
        }
}

} // namespace

extern "C" auto LLVMFuzzerTestOneInput(std::uint8_t const* data, std::size_t size)
        -> int
{
        auto const input = std::span(data, size);
        // Every base, and every length from empty up past the longest answer, the minimum in base 2.
        auto const base   = 2 + fuzz::trailer_at(input, 1UZ, 0UZ) % 35;
        auto const length = static_cast<std::size_t>(fuzz::trailer_at(input, 1UZ, 1UZ));
        std::apply([&]<class... T>(T...) -> void { (check_at(fuzz::operand<T>(input, 0UZ), base, length), ...); }, fuzz::integer_types{});
        return 0;
}
