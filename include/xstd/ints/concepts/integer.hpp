//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CONCEPTS_INTEGER_HPP
#define XSTD_INTS_CONCEPTS_INTEGER_HPP

#include <xstd/ints/concepts/integer_class.hpp>    // integer_class
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed_t
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <concepts>                                // same_as

namespace xstd {

// P3701R0's arithmetic boundary plus xstd's paired integer-class types; a character type is a third beside its pair.
template<class T>
concept integer =
        integer_class<T> and

        // The pair exists, which already rejects an unpaired integer-class type.
        requires {
                typename make_signed_t<T>;   // an integer-class type has one once make_signed is specialized, as its adaptor does
                typename make_unsigned_t<T>; // an integer-class type has one once make_unsigned is specialized, as its adaptor does
        } and

        // Each transformation is settled by signedness alone, not by the road taken.
        std::same_as<make_signed_t<make_unsigned_t<T>>, make_signed_t<T>> and
        std::same_as<make_unsigned_t<make_signed_t<T>>, make_unsigned_t<T>> and

        // T is one of its own pair rather than a third type beside it.
        (std::same_as<T, make_signed_t<T>> or std::same_as<T, make_unsigned_t<T>>); // a character type is neither half of its pair: char is not signed char

} // namespace xstd

#endif // XSTD_INTS_CONCEPTS_INTEGER_HPP
