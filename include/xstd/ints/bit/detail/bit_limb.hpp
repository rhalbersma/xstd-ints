//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BIT_DETAIL_BIT_LIMB_HPP
#define XSTD_INTS_BIT_DETAIL_BIT_LIMB_HPP

#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/limits/numeric_limits.hpp>            // numeric_limits
#include <cstddef>                                        // size_t
#include <limits>                                         // numeric_limits
#include <type_traits>                                    // conditional_t, is_class_v

namespace xstd::ints::detail {

// Every integer-class type converts to and from std::size_t, so every unsigned integer splits into these words.
using bit_limb = std::size_t;

inline constexpr auto bit_limb_width = static_cast<std::size_t>(std::numeric_limits<bit_limb>::digits);

// Unsigned for a builtin shift, so no count is signed; int for a class type's, as absl::uint128's operator<< takes.
template<class T>
using shift_count_t = std::conditional_t<std::is_class_v<T>, int, unsigned>;

// Each word is mirrored on its own and lands at the mirror of its offset, so a word's lowest unit becomes the highest.
template<class T, class Mirror>
[[nodiscard]] constexpr auto mirror_limbs(T x, Mirror mirror) noexcept(nothrow_const_operators<T>)
        -> T
{
        using shift_count      = shift_count_t<T>;
        constexpr auto N       = shift_count{xstd::numeric_limits<T>::digits};
        constexpr auto L       = shift_count{bit_limb_width};
        constexpr auto partial = N % L;
        auto result            = T{0};
        for (auto k = shift_count{0}; k < N - partial; k += L) {
                // Kept a bit_limb until shifted: Clang 19 crashes emitting a const local _BitInt over 128 bits.
                auto const mirrored = mirror(static_cast<bit_limb>(x >> k));
                result              = static_cast<T>(result | static_cast<T>(static_cast<T>(mirrored) << (N - L - k)));
        }
        if constexpr (partial != 0) {
                constexpr auto slack = unsigned{L - partial};
                auto const mirrored  = mirror(static_cast<bit_limb>(x >> (N - partial)));
                result               = static_cast<T>(result | static_cast<T>(mirrored >> slack));
        }
        return result;
}

} // namespace xstd::ints::detail

#endif // XSTD_INTS_BIT_DETAIL_BIT_LIMB_HPP
