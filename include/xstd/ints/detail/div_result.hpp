//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_DIV_RESULT_HPP
#define XSTD_INTS_DETAIL_DIV_RESULT_HPP

#include <xstd/ints/detail/integer.hpp> // integer

namespace xstd {

template<integer I>
struct div_result
{
        I quotient, remainder;

        // Both specifiers are implicit for a defaulted function; [[nodiscard]] is not.
        [[nodiscard]] friend auto operator==(div_result const&, div_result const&) -> bool = default;
};

// Deduction guide to keep -Wctad-maybe-unsupported quiet.
template<integer I>
div_result(I, I) -> div_result<I>;

} // namespace xstd

#endif // XSTD_INTS_DETAIL_DIV_RESULT_HPP
