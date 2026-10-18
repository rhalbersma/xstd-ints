//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CHARCONV_DETAIL_STD_WRITES_CHARS_HPP
#define XSTD_INTS_CHARCONV_DETAIL_STD_WRITES_CHARS_HPP

#include <xstd/ints/concepts/integer.hpp>        // integer
#include <xstd/ints/detail/delegates_to_std.hpp> // delegates_to_std
#include <charconv>                              // to_chars
#include <concepts>                              // same_as

namespace xstd::ints::detail {

template<class I>
concept std_writes_chars =
        delegates_to_std<I> and
        // [charconv.to.chars] covers the integer types and char; char8_t would reach it only by promotion.
        (std::same_as<I, char> or integer<I>) and
        // A library that admits _BitInt as integral may still give it no std::to_chars, as [charconv.syn] does.
        requires (char* first, I value, int base) { std::to_chars(first, first, value, base); };

} // namespace xstd::ints::detail

#endif // XSTD_INTS_CHARCONV_DETAIL_STD_WRITES_CHARS_HPP
