//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP
#define XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP

#include <xstd/ints/type_traits/promoted.hpp> // promoted_t
#include <concepts>                           // regular, same_as
#include <cstddef>                            // size_t
#include <type_traits>                        // remove_cv_t

// integer_class pruned to the bitwise half: what a type offers when it is a fixed-width field of bits rather
// than a number. std::bitset generalized exactly this set from the built-in integers -- the operators and
// nothing arithmetic -- so it is the set a bit container answers in, and the built-in widths answer in it too.
namespace xstd {

// The clause references are integer_class's, which this is pruned from; what is dropped is named where it was.
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept bitwise_operators =
        // cv-transparent, as integer_class is: every requirement below is stated of the cv-stripped T, so a
        // const, volatile or const volatile spelling models this exactly when the bare type does. The same_as
        // guards the defaulted parameter against being given something else.
        std::same_as<T, std::remove_cv_t<T_cv>> and

        // /9: regularity, which carries == and != with it. The strong ordering is NOT kept: std::bitset has no
        // operator< at all, so requiring an order would exclude the type this concept is shaped after.
        std::regular<T> and

        // /7.3: of the four unary operators only the complement is bitwise; +, - and ! are arithmetic or
        // contextual. Against promoted_t so a built-in subject to [conv.prom] still qualifies -- ~ on an
        // unsigned char yields int, not unsigned char.
        requires (T const a) {
                { ~a } -> std::same_as<promoted_t<T>>;
        } and

        // /7.5: same-type compound assignment, the shifts taking a width rather than a T. The arithmetic five,
        // *= /= %= += -=, are dropped; -= especially, because on an unsigned integer it is subtraction and on a
        // bit container it would read as set difference, and a concept cannot tell those apart.
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

// /2, /6, /7.1, /7.2, /7.4, /8, /10, /11 and /12 are all dropped: they are about being a NUMBER -- the
// numeric_limits interrogation, the conversions in and out, increment and decrement, the contextual conversion
// to bool, value initialization producing zero, min() and max(), and the mixed-mode common_type. A field of
// bits answers none of them and does not need to.

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP
