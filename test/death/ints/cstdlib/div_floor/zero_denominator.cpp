//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div_floor.hpp> // div_floor

// A zero denominator has no quotient to round.
auto main() -> int
{
        auto const volatile denom = 0;
        static_cast<void>(xstd::div_floor(7, denom));
}
