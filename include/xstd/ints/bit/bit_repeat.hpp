//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_BIT_REPEAT_HPP
#define XSTD_INTS_BIT_BIT_REPEAT_HPP

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/detail/bit_permutation.hpp>    // XSTD_HAS_STD_BIT_PERMUTATIONS, shift_count_t, std_permutes_bits
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <bit>                                     // bit_repeat
#include <cassert>                                 // assert
#include <utility>                                 // unreachable

// P3104R5's bit_repeat, over every unsigned integer xstd knows: a narrow contract, so not noexcept.
namespace xstd {

#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS

template<class T>
        requires ints::detail::std_permutes_bits<T>
[[nodiscard]] constexpr auto bit_repeat(T x, int l)
        -> T
{
        return std::bit_repeat(x, l);
}

#endif

// The pattern doubles until it fills the width, each copy landing a whole number of periods above the last.
template<unsigned_integer T>
        requires (not ints::detail::std_permutes_bits<T>)
[[nodiscard]] constexpr auto bit_repeat(T x, int l)
        -> T
{
        assert(l > 0);
        // P3104R6 [bit.permute]: a call violating the precondition is not a core constant expression, NDEBUG or not.
        if consteval {
                if (l <= 0) {
                        std::unreachable();
                }
        }
        using shift_count = ints::detail::shift_count_t<T>;
        constexpr auto N  = shift_count{numeric_limits<T>::digits};
        auto const period = static_cast<shift_count>(l);
        if (period >= N) {
                return x;
        }
        auto result = static_cast<T>(x & static_cast<T>(numeric_limits<T>::max() >> (N - period)));
        for (auto filled = period; filled < N; filled += filled) {
                result = static_cast<T>(result | static_cast<T>(result << filled));
        }
        return result;
}

} // namespace xstd

#endif // XSTD_INTS_BIT_BIT_REPEAT_HPP
