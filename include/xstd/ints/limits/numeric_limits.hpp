//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_LIMITS_NUMERIC_LIMITS_HPP
#define XSTD_INTS_LIMITS_NUMERIC_LIMITS_HPP

#include <xstd/ints/limits/detail/digits10.hpp> // digits10, digits10_exact_width
#include <cstddef>                              // size_t
#include <limits>                               // numeric_limits
#include <type_traits>                          // is_array_v, is_object_v

namespace xstd {

// Says nothing, as make_signed's primary does; empty not just declared, the cv specializations below inheriting it.
template<class T>
struct numeric_limits
{};

// Open counterpart of std::numeric_limits, whose members an array or a function type cannot return.
template<class T>
        requires (std::is_object_v<T> and not std::is_array_v<T>)
struct numeric_limits<T> : std::numeric_limits<T>
{};

template<class T>
struct numeric_limits<T const> : numeric_limits<T>
{};

template<class T>
struct numeric_limits<T volatile> : numeric_limits<T>
{};

template<class T>
struct numeric_limits<T const volatile> : numeric_limits<T>
{};

#ifdef __BITINT_MAXWIDTH__

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbit-int-extension"
#endif

static_assert(__BITINT_MAXWIDTH__ <= ints::detail::digits10_exact_width);

// Beside the primary, since any program can spell _BitInt; libstdc++ has none, libc++ takes digits from sizeof.
template<std::size_t N>
struct numeric_limits<signed _BitInt(N)> : std::numeric_limits<signed _BitInt(N)>
{
        using type          = signed _BitInt(N);
        using unsigned_type = unsigned _BitInt(N);

        static constexpr auto is_specialized = true;
        static constexpr auto is_signed      = true;
        static constexpr auto is_integer     = true;
        static constexpr auto is_exact       = true;
        static constexpr auto is_bounded     = true;
        static constexpr auto is_modulo      = false;
        static constexpr auto digits         = static_cast<int>(N) - 1;
        static constexpr auto digits10       = ints::detail::digits10(digits);
        static constexpr auto radix          = 2;

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
        static constexpr auto is_signed      = false;
        static constexpr auto is_integer     = true;
        static constexpr auto is_exact       = true;
        static constexpr auto is_bounded     = true;
        static constexpr auto is_modulo      = true;
        static constexpr auto digits         = static_cast<int>(N);
        static constexpr auto digits10       = ints::detail::digits10(digits);
        static constexpr auto radix          = 2;

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

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // __BITINT_MAXWIDTH__

} // namespace xstd

#endif // XSTD_INTS_LIMITS_NUMERIC_LIMITS_HPP
