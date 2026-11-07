//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint/bit_int.hpp>       // bit_int, bit_uint, XSTD_HAS_BIT_INT
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <cstdint>                             // int32_t, uint8_t
#include <iostream>                            // cout

// A type without limits has no members to name, so asking is a substitution failure rather than a hard error.
template<class T>
concept has_max = requires { xstd::numeric_limits<T>::max(); };

static_assert(has_max<std::int32_t const>);
static_assert(not has_max<std::int32_t&>);
static_assert(not has_max<std::int32_t[4]>);
static_assert(not has_max<void>);

#ifdef XSTD_HAS_BIT_INT
// A bit-precise type has the limits of its requested width, not of the bytes that store it, cv-qualified or not.
static_assert(xstd::numeric_limits<xstd::bit_int<17>>::digits == 16);
static_assert(xstd::numeric_limits<xstd::bit_int<17> const>::max() == 65535);
static_assert(xstd::numeric_limits<xstd::bit_uint<17> const>::max() == 131071);
static_assert(xstd::numeric_limits<xstd::bit_uint<24>>::digits10 == 7);
#endif

template<class T>
auto show(char const* name)
        -> void
{
        using limits = xstd::numeric_limits<T>;
        std::cout << name << ": digits " << limits::digits << ", digits10 " << limits::digits10 << ", min " << +limits::min() << ", max " << +limits::max() << '\n';
}

auto main() -> int
{
        // The type of a const variable is const-qualified, and its limits are those of the unqualified type.
        auto const count = std::uint8_t{42};
        show<decltype(count)>("uint8_t const");
        show<std::int32_t volatile>("int32_t volatile");
}
