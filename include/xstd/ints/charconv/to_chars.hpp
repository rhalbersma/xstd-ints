//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CHARCONV_TO_CHARS_HPP
#define XSTD_INTS_CHARCONV_TO_CHARS_HPP

#include <xstd/ints/concepts/integer.hpp>          // integer
#include <xstd/ints/cstdlib/div.hpp>               // div
#include <xstd/ints/detail/delegates_to_std.hpp>   // delegates_to_std
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp>     // is_signed_v
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <cassert>                                 // assert
#include <charconv>                                // to_chars, to_chars_result
#include <concepts>                                // integral, same_as
#include <cstddef>                                 // ptrdiff_t, size_t
#include <iterator>                                // distance, next
#include <system_error>                            // errc
#include <type_traits>                             // remove_cv_t

namespace xstd {

// Worst case is base 2: one character per value bit, and two more when signed.
template<class I>
        requires ((std::integral<I> and not std::same_as<std::remove_cv_t<I>, bool>) or integer<I>)
inline constexpr auto to_chars_max_size =
        static_cast<std::size_t>(numeric_limits<I>::digits) + (is_signed_v<I> ? 2 : 0);

// The standard's own integral domain, other than its deleted bool overload.
template<std::integral I>
        requires (not std::same_as<std::remove_cv_t<I>, bool>) and ints::detail::delegates_to_std<I>
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10)
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);
        return std::to_chars(first, last, value, base);
}

// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base
auto to_chars(char*, char*, bool, int = 10) -> std::to_chars_result = delete;

// For xstd integers the standard library does not cover.
template<integer I>
        requires (not ints::detail::delegates_to_std<I>)
// NOLINTNEXTLINE(readability-magic-numbers): the standard's own default base, see above
[[nodiscard]] constexpr auto to_chars(char* first, char* last, I value, int base = 10)
        -> std::to_chars_result
{
        assert(2 <= base and base <= 36);

        // A pointer rather than a string_view: these are only ever subscripted.
        static constexpr auto* digits = "0123456789abcdefghijklmnopqrstuvwxyz";

        // The unsigned counterpart holds |min()| and lets both loops stop at the radix.
        using U = make_unsigned_t<I>;
        auto const radix = static_cast<U>(base);

        // Reduced once here, not per digit; both are well-formed and branchless when unsigned.
        auto const zero = static_cast<I>(0);
        auto const negative = value < zero;
        auto const magnitude = static_cast<U>(negative ? U{} - static_cast<U>(value) : static_cast<U>(value));

        // Converted rather than selected: a conditional would be one-sided when unsigned.
        auto const sign_width = static_cast<std::ptrdiff_t>(negative);

        // Every value writes a digit, and a negative one a sign before it.
        if (std::distance(first, last) < sign_width + 1) {
                return {.ptr = last, .ec = std::errc::value_too_large};
        }

        // The walk claims a position per digit, so a step onto last is the short buffer.
        auto* out = first + sign_width;
        // "rest / radix" not "/=": absl::int128 is constexpr on the first only.
        for (auto rest = magnitude; rest >= radix; rest = rest / radix) {
                if (++out == last) {
                        return {.ptr = last, .ec = std::errc::value_too_large};
                }
        }

        // Taken while out still means the last digit, before the write walks it back down.
        auto const result = std::to_chars_result{.ptr = std::next(out), .ec = std::errc{}};

        // One divmod per digit, named by the library's own truncated division.
        auto rest = magnitude;
        while (rest >= radix) {
                auto const [quotient, remainder] = xstd::div(rest, radix);
                *out-- = digits[static_cast<std::size_t>(remainder)];
                rest = quotient;
        }
        // The loop leaves a single digit. No decrement: unsigned, it would step below first.
        *out = digits[static_cast<std::size_t>(rest)];

        // Back into the position the walk reserved; here the sign is a test, not an offset.
        if constexpr (is_signed_v<I>) {
                if (negative) {
                        *--out = '-';
                }
        }
        return result;
}

} // namespace xstd

#endif // XSTD_INTS_CHARCONV_TO_CHARS_HPP
