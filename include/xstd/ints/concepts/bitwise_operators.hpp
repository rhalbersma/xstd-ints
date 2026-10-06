//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP
#define XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP

#include <xstd/ints/concepts/bitmask_type.hpp> // bitmask_type
#include <xstd/ints/type_traits/promoted.hpp>  // promoted_t
#include <concepts>                            // same_as
#include <cstddef>                             // size_t
#include <type_traits>                         // remove_cv_t

// integer_class pruned to the bitwise half: the operator set std::bitset shares with the built-in integers.
namespace xstd {

// The clause references are integer_class's, which this is pruned from; what is dropped is named where it was.
template<class T_cv, class T = std::remove_cv_t<T_cv>>
concept bitwise_operators =
        // cv-transparent: every requirement is stated of the cv-stripped T, same_as guarding the defaulted parameter.
        std::same_as<T, std::remove_cv_t<T_cv>> and

        // A bitmask type, which brings the unsigned domain, /9's regularity, /7.3's ~ and /7.6's & ^ |, all non-shifts.
        bitmask_type<T_cv> and

        // /7.5: same-type compound assignment returning T&, the shifts taking a width; the arithmetic five go.
        requires (T a, std::size_t const n) {
                { a <<= n } -> std::same_as<T&>;
                { a >>= n } -> std::same_as<T&>;
        } and
        requires (T a, T const b) {
                { a &= b } -> std::same_as<T&>;
                { a ^= b } -> std::same_as<T&>;
                { a |= b } -> std::same_as<T&>;
        } and

        // /7.6: the binary shifts, against promoted_t so [conv.prom] built-ins qualify.
        requires (T const a, std::size_t const n) {
                { a << n } -> std::same_as<promoted_t<T>>;
                { a >> n } -> std::same_as<promoted_t<T>>;
        };

// /2, /6, /7.1, /7.2, /7.4, /8, /10, /11 and /12 are dropped: they are about being a NUMBER, not a field of bits.

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_BITWISE_OPERATORS_HPP
