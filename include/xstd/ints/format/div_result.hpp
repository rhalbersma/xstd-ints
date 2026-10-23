//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_FORMAT_DIV_RESULT_HPP
#define XSTD_INTS_FORMAT_DIV_RESULT_HPP

#include <xstd/ints/charconv/to_chars.hpp>     // to_chars, to_chars_max_size
#include <xstd/ints/concepts/integer.hpp>      // integer
#include <xstd/ints/cstdlib/div_result.hpp>    // div_result
#include <xstd/ints/format/detail/decimal.hpp> // decimal, formats_tuples
#include <array>                               // array
#include <cassert>                             // assert
#include <format>                              // formattable, formatter
#include <string>                              // basic_string
#include <string_view>                         // basic_string_view
#include <system_error>                        // errc
#include <tuple>                               // tie, tuple

// A div_result renders as "(quotient, remainder)": by the tuple formatter, or by xstd::to_chars.

// The base's format() is not constexpr before P3391 (C++29); claiming it anyway is IFNDR.
#if defined(__cpp_lib_constexpr_format) && __cpp_lib_constexpr_format >= 202511L
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

// NOLINTBEGIN(bugprone-std-namespace-modification): [namespace.std]/2 admits specializing for a program-defined type.

namespace std {

// The one every renderable div_result matches, left where no tuple formats; the string base carries the spec.
template<xstd::integer I, class CharT>
struct formatter<xstd::div_result<I>, CharT> : formatter<basic_string_view<CharT>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_result<I> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                constexpr auto N = xstd::to_chars_max_size<I>;
                auto widened     = std::basic_string<CharT>{};
                auto buffer      = std::array<char, N>{};

                auto const append = [&](I const value) -> void {
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

                return formatter<basic_string_view<CharT>, CharT>::format(widened, ctx);
        }
};

// libstdc++'s tuple formatter over stateless elements ends in tail padding, which Clang's -Wpadded reports.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

// Where the library formats tuples but not I, the digits go through a tuple, so the spec grammar does not depend on I.
template<xstd::integer I, class CharT>
        requires xstd::ints::detail::formats_tuples<CharT>
struct formatter<xstd::div_result<I>, CharT> : formatter<tuple<xstd::ints::detail::decimal, xstd::ints::detail::decimal>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_result<I> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                constexpr auto N = xstd::to_chars_max_size<I>;
                auto quotient    = std::array<char, N>{};
                auto remainder   = std::array<char, N>{};
                auto const q     = xstd::to_chars(quotient.data(), quotient.data() + N, d.quotient);
                auto const r     = xstd::to_chars(remainder.data(), remainder.data() + N, d.remainder);
                // The buffers are sized for base 2, so decimal always fits.
                assert(q.ec == std::errc{} and r.ec == std::errc{});
                auto const digits = tuple{
                        xstd::ints::detail::decimal{.digits = {quotient.data(), q.ptr}},
                        xstd::ints::detail::decimal{.digits = {remainder.data(), r.ptr}}
                };
                return formatter<tuple<xstd::ints::detail::decimal, xstd::ints::detail::decimal>, CharT>::format(digits, ctx);
        }
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

// The most constrained one: where the library formats I as well, the tuple formatter renders the members themselves.
template<xstd::integer I, class CharT>
        requires xstd::ints::detail::formats_tuples<CharT> and formattable<tuple<I const&, I const&>, CharT>
struct formatter<xstd::div_result<I>, CharT> : formatter<tuple<I const&, I const&>, CharT>
{
        [[nodiscard]] XSTD_CONSTEXPR_FORMAT auto format(xstd::div_result<I> const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                // tie yields the base's own type, so nothing is copied on the way in.
                return formatter<tuple<I const&, I const&>, CharT>::format(std::tie(d.quotient, d.remainder), ctx);
        }
};

} // namespace std

// NOLINTEND(bugprone-std-namespace-modification)

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_INTS_FORMAT_DIV_RESULT_HPP
