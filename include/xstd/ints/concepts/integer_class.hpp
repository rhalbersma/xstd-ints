//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_INTEGER_CLASS_HPP
#define XSTD_INTS_CONCEPTS_INTEGER_CLASS_HPP

#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <xstd/ints/type_traits/promoted.hpp>  // promoted_t
#include <compare>                             // strong_ordering
#include <concepts>                            // constructible_from, convertible_to, regular, same_as, three_way_comparable
#include <cstddef>                             // size_t
#include <type_traits>                         // remove_cv_t

// [iterator.concept.winc]'s integer-class type, generalized to integers of any width.
namespace xstd {

template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept integer_class =
        // /2: structurally recognize the set; its note disclaims class-ness, so built-ins are eligible.
        std::same_as<T, std::remove_cv_t<T_cv>> and

        // /3: a width exists, which no arbitrary-precision integer has; wider than the integral types is not asked.
        numeric_limits<T>::is_specialized and
        numeric_limits<T>::is_integer and
        numeric_limits<T>::is_bounded and
        (numeric_limits<T>::radix == 2) and

        // /4: integer-like and its signedness refinements are defined from this concept.

        // /5: B(T) is a specification device and has no structural check.

        // /6: representative implicit conversions in and explicit conversions out.
        std::convertible_to<int, T> and
        std::convertible_to<std::size_t, T> and
        std::constructible_from<int, T> and
        std::constructible_from<std::size_t, T> and

        // /7.1: postfix increment and decrement.
        requires (T a) {
                { a++ } -> std::same_as<T>;
                { a-- } -> std::same_as<T>;
        } and

        // /7.2: prefix increment and decrement.
        requires (T a) {
                { ++a } -> std::same_as<T&>;
                { --a } -> std::same_as<T&>;
        } and

        // /7.3: unary operators, against promoted_t so a built-in subject to [conv.prom] still qualifies.
        requires (T const a) {
                { +a } -> std::same_as<promoted_t<T>>;
                { -a } -> std::same_as<promoted_t<T>>;
                // NOLINTNEXTLINE(readability-operators-representation): /7.3 spells this one !
                { !a } -> std::same_as<bool>;
                { ~a } -> std::same_as<promoted_t<T>>;
        } and

        // /7.4: xstd does not require mixed integral/integer-class compound assignment.

        // /7.5: same-type compound assignment, grouped by precedence and operand arity.
        requires (T a, T const b) {
                { a *= b } -> std::same_as<T&>;
                { a /= b } -> std::same_as<T&>;
                { a %= b } -> std::same_as<T&>;
                { a += b } -> std::same_as<T&>;
                { a -= b } -> std::same_as<T&>;
        } and
        requires (T a, std::size_t const n) {
                { a <<= n } -> std::same_as<T&>;
                { a >>= n } -> std::same_as<T&>;
        } and
        requires (T a, T const b) {
                { a &= b } -> std::same_as<T&>;
                { a ^= b } -> std::same_as<T&>;
                { a |= b } -> std::same_as<T&>;
        } and

        // /7.6: same-type binary operators, against promoted_t for the reason /7.3 is.
        requires (T const a, T const b) {
                { a * b } -> std::same_as<promoted_t<T>>;
                { a / b } -> std::same_as<promoted_t<T>>;
                { a % b } -> std::same_as<promoted_t<T>>;
                { a + b } -> std::same_as<promoted_t<T>>;
                { a - b } -> std::same_as<promoted_t<T>>;
        } and
        requires (T const a, std::size_t const n) {
                { a << n } -> std::same_as<promoted_t<T>>;
                { a >> n } -> std::same_as<promoted_t<T>>;
        } and
        requires (T const a, T const b) {
                { a & b } -> std::same_as<promoted_t<T>>;
                { a ^ b } -> std::same_as<promoted_t<T>>;
                { a | b } -> std::same_as<promoted_t<T>>;
        } and

        // /8: contextually convertible to bool; an explicit conversion is sufficient.
        std::constructible_from<bool, T> and

        // /9: regularity and strong ordering.
        std::regular<T> and
        std::three_way_comparable<T, std::strong_ordering> and

        // /10: semantic requirement — value initialization produces zero.

        // /11: the integer range functions return T.
        requires {
                { numeric_limits<T>::min() } -> std::same_as<T>;
                { numeric_limits<T>::max() } -> std::same_as<T>;
        };

// /12: xstd does not require mixed-mode common_type relationships.

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_INTEGER_CLASS_HPP
