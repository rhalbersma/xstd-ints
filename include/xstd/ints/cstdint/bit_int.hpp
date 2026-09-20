//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_CSTDINT_BIT_INT_HPP
#define XSTD_INTS_CSTDINT_BIT_INT_HPP

#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <xstd/ints/type_traits/make_signed.hpp>   // make_signed
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned
#include <cstddef>                                 // size_t
#include <limits>                                  // numeric_limits
#include <type_traits>                             // type_identity

#ifdef __BITINT_MAXWIDTH__

#define XSTD_HAS_BIT_INT 1

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbit-int-extension"
#endif

namespace xstd {

inline constexpr auto bit_int_max_width = static_cast<std::size_t>(__BITINT_MAXWIDTH__);

// Keep the public domain paired until Clang implements signed _BitInt(1) from N3747.
template<std::size_t N>
        requires (2 <= N and N <= bit_int_max_width)
using bit_int = signed _BitInt(N);

template<std::size_t N>
        requires (2 <= N and N <= bit_int_max_width)
using bit_uint = unsigned _BitInt(N);

// Not deferred to std: libc++ specializes these, but takes digits from sizeof.
template<std::size_t N>
struct numeric_limits<signed _BitInt(N)> : std::numeric_limits<signed _BitInt(N)>
{
        using type = signed _BitInt(N);
        using unsigned_type = unsigned _BitInt(N);

        static constexpr auto is_specialized = true;
        static constexpr auto is_signed = true;
        static constexpr auto is_integer = true;
        static constexpr auto is_exact = true;
        static constexpr auto is_bounded = true;
        static constexpr auto is_modulo = false;
        static constexpr auto digits = static_cast<int>(N) - 1;
        static constexpr auto radix = 2;

        [[nodiscard]] static constexpr auto min() noexcept
                -> type
        {
                return static_cast<type>(-max() - type{1});
        }

        [[nodiscard]] static constexpr auto max() noexcept
                -> type
        {
                return static_cast<type>(static_cast<unsigned_type>(~unsigned_type{0}) >> 1U);
        }

        [[nodiscard]] static constexpr auto lowest() noexcept
                -> type
        {
                return min();
        }
};

template<std::size_t N>
struct numeric_limits<unsigned _BitInt(N)> : std::numeric_limits<unsigned _BitInt(N)>
{
        using type = unsigned _BitInt(N);

        static constexpr auto is_specialized = true;
        static constexpr auto is_signed = false;
        static constexpr auto is_integer = true;
        static constexpr auto is_exact = true;
        static constexpr auto is_bounded = true;
        static constexpr auto is_modulo = true;
        static constexpr auto digits = static_cast<int>(N);
        static constexpr auto radix = 2;

        [[nodiscard]] static constexpr auto min() noexcept
                -> type
        {
                return type{0};
        }

        [[nodiscard]] static constexpr auto max() noexcept
                -> type
        {
                return static_cast<type>(~type{0});
        }

        [[nodiscard]] static constexpr auto lowest() noexcept
                -> type
        {
                return min();
        }
};

template<std::size_t N>
struct make_unsigned<signed _BitInt(N)> : std::type_identity<unsigned _BitInt(N)>
{};

template<std::size_t N>
struct make_signed<unsigned _BitInt(N)> : std::type_identity<signed _BitInt(N)>
{};

} // namespace xstd

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // __BITINT_MAXWIDTH__

#endif // XSTD_INTS_CSTDINT_BIT_INT_HPP
