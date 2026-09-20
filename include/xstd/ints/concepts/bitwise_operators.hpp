//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP
#define XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP

#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/type_traits/promoted.hpp>      // promoted_t
#include <concepts>                                // integral, regular, same_as
#include <cstddef>                                 // size_t
#include <type_traits>                             // remove_cv_t

// integer_class pruned to the bitwise half: the operator set std::bitset shares with the built-in integers.
namespace xstd {

// The clause references are integer_class's, which this is pruned from; what is dropped is named where it was.
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept bitwise_operators =
        // cv-transparent: every requirement is stated of the cv-stripped T, same_as guarding the defaulted parameter.
        std::same_as<T, std::remove_cv_t<T_cv>> and

        // An integer must be unsigned; a non-integer is judged by its operators alone, which admits std::bitset.
        (unsigned_integer<T> or (not signed_integer<T> and not std::integral<T>)) and

        // /9: regularity, carrying == and != with it; no strong ordering, std::bitset having no operator< to require.
        std::regular<T> and

        // /7.3: of the four unary operators only ~ is bitwise, against promoted_t so [conv.prom] built-ins qualify.
        requires (T const a) {
                { ~a } -> std::same_as<promoted_t<T>>;
        } and

        // /7.5: same-type compound assignment, the shifts taking a width; the arithmetic five go.
        requires (T a, std::size_t const n) {
                { a <<= n } -> std::same_as<T&>;
                { a >>= n } -> std::same_as<T&>;
        } and
        requires (T a, T const b) {
                { a &= b } -> std::same_as<T&>;
                { a ^= b } -> std::same_as<T&>;
                { a |= b } -> std::same_as<T&>;
        } and

        // /7.6: the binary forms of the same three groups, against promoted_t for the reason /7.3 is.
        requires (T const a, std::size_t const n) {
                { a << n } -> std::same_as<promoted_t<T>>;
                { a >> n } -> std::same_as<promoted_t<T>>;
        } and
        requires (T const a, T const b) {
                { a & b } -> std::same_as<promoted_t<T>>;
                { a ^ b } -> std::same_as<promoted_t<T>>;
                { a | b } -> std::same_as<promoted_t<T>>;
        };

// /2, /6, /7.1, /7.2, /7.4, /8, /10, /11 and /12 are dropped: they are about being a NUMBER, not a field of bits.

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP
