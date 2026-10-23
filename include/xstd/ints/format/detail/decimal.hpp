//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_FORMAT_DETAIL_DECIMAL_HPP
#define XSTD_INTS_FORMAT_DETAIL_DECIMAL_HPP

#include <format>      // basic_format_parse_context, formattable, formatter
#include <string_view> // string_view
#include <tuple>       // tuple

#if defined(__cpp_lib_constexpr_format) && __cpp_lib_constexpr_format >= 202511L
#define XSTD_CONSTEXPR_FORMAT constexpr
#else
#define XSTD_CONSTEXPR_FORMAT
#endif

namespace xstd::ints::detail {

// An integer that to_chars has already rendered, for a tuple formatter to place as one of its elements.
struct decimal
{
        std::string_view digits;
};

} // namespace xstd::ints::detail

// NOLINTBEGIN(bugprone-std-namespace-modification): [namespace.std]/2 admits specializing for a program-defined type.

namespace std {

// No [[nodiscard]] on either member: the Microsoft STL's tuple formatter discards what each element's returns.
template<class CharT>
struct formatter<xstd::ints::detail::decimal, CharT>
{
        // The tuple formatter hands each element an empty spec: there is nothing to parse.
        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
                -> basic_format_parse_context<CharT>::iterator
        {
                return ctx.begin();
        }

        XSTD_CONSTEXPR_FORMAT auto format(xstd::ints::detail::decimal const& d, auto& ctx) const
                -> decltype(ctx.out())
        {
                auto out = ctx.out();
                for (auto const c : d.digits) {
                        *out++ = static_cast<CharT>(c);
                }
                return out;
        }
};

} // namespace std

// NOLINTEND(bugprone-std-namespace-modification)

namespace xstd::ints::detail {

// The standard library formats tuples, so a tuple's spec grammar is there to borrow.
template<class CharT>
concept formats_tuples = std::formattable<std::tuple<decimal, decimal>, CharT>;

} // namespace xstd::ints::detail

#undef XSTD_CONSTEXPR_FORMAT

#endif // XSTD_INTS_FORMAT_DETAIL_DECIMAL_HPP
