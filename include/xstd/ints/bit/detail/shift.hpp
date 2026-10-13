//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_SHIFT_HPP
#define XSTD_INTS_BIT_DETAIL_SHIFT_HPP

#include <xstd/ints/bit/detail/bit_block.hpp>             // shift_count_t
#include <xstd/ints/concepts/integer.hpp>                 // integer
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdlib/unsigned_abs.hpp>             // unsigned_abs
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp>            // is_signed_v
#include <xstd/ints/type_traits/make_unsigned.hpp>        // make_unsigned_t

namespace xstd::ints::detail {

enum class shift_direction : bool {
        left,
        right,
};

// Below zero only where the type has a sign, asked without comparing an unsigned value to zero.
template<integer T>
[[nodiscard]] constexpr auto is_negative(T x) noexcept(nothrow_const_operators<T>)
        -> bool
{
        if constexpr (is_signed_v<T>) {
                return x < T{0};
        } else {
                return false;
        }
}

// Whether a count's magnitude reaches the width, converting whichever of the two the other's type holds.
template<class U>
[[nodiscard]] constexpr auto reaches_width(U magnitude, int width) noexcept(nothrow_const_operators<U>)
        -> bool
{
        if constexpr (numeric_limits<U>::digits < numeric_limits<int>::digits) {
                return static_cast<int>(magnitude) >= width;
        } else {
                return magnitude >= static_cast<U>(width);
        }
}

// P3793's x times two to the power of a signed count, rounded towards negative infinity and reduced modulo the width.
template<integer T, integer S>
[[nodiscard]] constexpr auto shift(T x, S s, shift_direction direction) noexcept(nothrow_const_operators<T> and nothrow_const_operators<S>)
        -> T
{
        using U              = make_unsigned_t<T>;
        auto const leftward  = (direction == shift_direction::left) != is_negative(s);
        auto const magnitude = xstd::unsigned_abs(s);
        if (reaches_width(magnitude, numeric_limits<U>::digits)) {
                return leftward or not is_negative(x) ? T{0} : static_cast<T>(-1);
        }
        // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
        auto const& u = static_cast<U>(x);
        auto const n  = static_cast<shift_count_t<U>>(magnitude);
        if (leftward) {
                return static_cast<T>(static_cast<U>(u << n));
        }
        // A negative value floors by shifting its complement, whose ones are the zeros it lacks.
        return is_negative(x) ? static_cast<T>(static_cast<U>(~static_cast<U>(static_cast<U>(~u) >> n))) : static_cast<T>(static_cast<U>(u >> n));
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_SHIFT_HPP
