//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_BIT_MASK_HPP
#define XSTD_INTS_CONCEPTS_BIT_MASK_HPP

#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/type_traits/promoted.hpp>      // promoted_t
#include <concepts>                                // integral, regular, same_as
#include <type_traits>                             // remove_cv_t

// [bitmask.types]'s bitmask type: an unsigned integer, std::bitset, or an enumeration overloading the operators.
namespace xstd {

template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept bit_mask =
        // cv-transparent: every requirement is stated of the cv-stripped T, same_as guarding the defaulted parameter.
        std::same_as<T, std::remove_cv_t<T_cv>> and

        // /1's integer types narrowed to the unsigned ones; a non-integer is judged by its operators alone.
        (unsigned_integer<T> or (not signed_integer<T> and not std::integral<T>)) and

        // /4: Y is set in X when X & Y is nonzero, which takes equality and a zero, T{}.
        std::regular<T> and

        // /2: the complement, against promoted_t so [conv.prom] built-ins qualify.
        requires (T const a) {
                { ~a } -> std::same_as<promoted_t<T>>;
        } and

        // /2: same-type compound assignment, returning T& as the synopsis writes it.
        requires (T a, T const b) {
                { a &= b } -> std::same_as<T&>;
                { a ^= b } -> std::same_as<T&>;
                { a |= b } -> std::same_as<T&>;
        } and

        // /2: the binary forms, against promoted_t so [conv.prom] built-ins qualify.
        requires (T const a, T const b) {
                { a & b } -> std::same_as<promoted_t<T>>;
                { a ^ b } -> std::same_as<promoted_t<T>>;
                { a | b } -> std::same_as<promoted_t<T>>;
        };

// No shifts: [bitmask.types] names none, and an enumeration overloading the rest need not add them.

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_BIT_MASK_HPP
