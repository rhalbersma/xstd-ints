//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_NOTHROW_CONST_OPERATORS_HPP
#define XSTD_INTS_DETAIL_NOTHROW_CONST_OPERATORS_HPP

#include <xstd/ints/detail/integer.hpp> // integer
#include <concepts>                     // same_as
#include <cstddef>                      // size_t
#include <type_traits>                  // is_nothrow_constructible_v, remove_cv_t

namespace xstd {

// Whether a type's const operations all carry noexcept, asked of T with the cv stripped.
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept nothrow_const_operators =
        // T is the parameter's own default; naming it explicitly cannot redirect the question.
        std::same_as<T, std::remove_cv_t<T_cv>> and
        integer<T> and
        // /9's regularity, spent on every call; not the assignment, swap or default init nothing here performs.
        std::is_nothrow_destructible_v<T> and
        std::is_nothrow_move_constructible_v<T> and
        std::is_nothrow_copy_constructible_v<T> and
        // /6, where integer_class asks it: the two conversions in, then the two out.
        std::is_nothrow_constructible_v<T, int> and
        std::is_nothrow_constructible_v<T, std::size_t> and
        std::is_nothrow_constructible_v<int, T> and
        std::is_nothrow_constructible_v<std::size_t, T> and
        requires (T const a) {
                // Asked as written: /7.3 and /7.6 give the result type already, so a cast adds nothing.
                { +a } noexcept;
                { -a } noexcept;
                // NOLINTNEXTLINE(readability-operators-representation): /7.3 spells this one !
                { !a } noexcept;
                { ~a } noexcept;
        } and
        requires (T const a, T const b) {
                { a * b } noexcept;
                { a / b } noexcept;
                { a % b } noexcept;
                { a + b } noexcept;
                { a - b } noexcept;
        } and
        requires (T const a, std::size_t const n) {
                { a << n } noexcept;
                { a >> n } noexcept;
        } and
        requires (T const a, T const b) {
                { a <=> b } noexcept;
                { a < b } noexcept;
                { a > b } noexcept;
                { a <= b } noexcept;
                { a >= b } noexcept;
                { a == b } noexcept;
                { a != b } noexcept;
                { a & b } noexcept;
                { a ^ b } noexcept;
                { a | b } noexcept;
        } and

        // /8, likewise: the contextual conversion to bool, after the operators.
        std::is_nothrow_constructible_v<bool, T>;

} // namespace xstd

#endif // XSTD_INTS_DETAIL_NOTHROW_CONST_OPERATORS_HPP
