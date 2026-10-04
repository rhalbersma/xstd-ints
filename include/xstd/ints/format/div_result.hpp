//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_FORMAT_DIV_RESULT_HPP
#define XSTD_INTS_FORMAT_DIV_RESULT_HPP

#include <xstd/ints/charconv/to_chars.hpp>  // to_chars, to_chars_max_size
#include <xstd/ints/concepts/integer.hpp>   // integer
#include <xstd/ints/cstdlib/div_result.hpp> // div_result
#include <array>                            // array
#include <cassert>                          // assert
#include <format>                           // formattable, formatter
#include <string>                           // basic_string
#include <string_view>                      // basic_string_view
#include <system_error>                     // errc
#include <tuple>                            // tie, tuple

// A div_result renders as "(quotient, remainder)": by the tuple formatter, or by xstd::to_chars.

// The base's format() is not constexpr before P3391 (C++29); claiming it anyway is IFNDR.
#if defined(__cpp_lib_constexpr_format) && __cpp_lib_constexpr_format >= 202511L
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

// The one every renderable div_result matches; the string base carries fill, alignment and width.
template<xstd::integer I, class CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2, see above
struct std::formatter<xstd::div_result<I>, CharT> : std::formatter<std::basic_string_view<CharT>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_result<I> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                constexpr auto N = xstd::to_chars_max_size<I>;
                auto widened     = std::basic_string<CharT>{};
                auto buffer      = std::array<char, N>{};

                auto const append = [&](I const value) XSTD_CONSTEXPR_FORMAT -> void {
                        auto const result = xstd::to_chars(buffer.data(), buffer.data() + N, value);
                        // The buffer is sized for base 2, so decimal always fits.
                        assert(result.ec == std::errc{});
                        for (auto const* p = buffer.data(); p != result.ptr; ++p) {
                                widened.push_back(static_cast<CharT>(*p));
                        }
                };

                // Spelled to match the tuple formatter, so the two render identically.
                widened.push_back(static_cast<CharT>('('));
                append(d.quotient);
                widened.push_back(static_cast<CharT>(','));
                widened.push_back(static_cast<CharT>(' '));
                append(d.remainder);
                widened.push_back(static_cast<CharT>(')'));

                return std::formatter<std::basic_string_view<CharT>, CharT>::format(widened, ctx);
        }
};

// The more constrained one: asked after the tuple, which covers both ways it can be absent.
template<xstd::integer I, class CharT>
        requires std::formattable<std::tuple<I const&, I const&>, CharT>
// NOLINTNEXTLINE(bugprone-std-namespace-modification): permitted by [namespace.std]/2, see above
struct std::formatter<xstd::div_result<I>, CharT> : std::formatter<std::tuple<I const&, I const&>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_result<I> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                // tie yields the base's own type, so nothing is copied on the way in.
                return std::formatter<std::tuple<I const&, I const&>, CharT>::format(std::tie(d.quotient, d.remainder), ctx);
        }
};

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_INTS_FORMAT_DIV_RESULT_HPP
