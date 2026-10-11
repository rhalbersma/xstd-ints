//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_CEIL_HPP
#define XSTD_INTS_BIT_BIT_CEIL_HPP

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/bit/bit_width.hpp>             // bit_width
#include <xstd/ints/detail/bit_limb.hpp>           // shift_count_t
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <bit>                                     // bit_ceil
#include <cassert>                                 // assert
#include <utility>                                 // unreachable

// [bit.pow.two]'s bit_ceil over every unsigned integer xstd knows: a narrow contract, so not noexcept.
namespace xstd {

// Constrained on the call, so constraint and body cannot drift: std::unsigned_integral admits what <bit> refuses.
template<class T>
        requires requires (T x) { std::bit_ceil(x); }
[[nodiscard]] constexpr auto bit_ceil(T x)
        -> T
{
        return std::bit_ceil(x);
}

// The lowest power of two not below x, which the width must hold.
template<unsigned_integer T>
        requires (not requires (T x) { std::bit_ceil(x); })
[[nodiscard]] constexpr auto bit_ceil(T x)
        -> T
{
        // Zero and one share the empty width; LLVM 18 turns a select on x <= 1 into a power-of-two mask.
        auto const below = x == T{0} ? x : static_cast<T>(x - T{1});
        auto const exponent = xstd::bit_width(below);
        assert(exponent < numeric_limits<T>::digits);
        // [bit.pow.two]: a call violating the precondition is not a core constant expression, NDEBUG or not.
        if consteval {
                if (exponent >= numeric_limits<T>::digits) {
                        std::unreachable();
                }
        }
        return static_cast<T>(T{1} << static_cast<ints::detail::shift_count_t<T>>(exponent));
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_CEIL_HPP
