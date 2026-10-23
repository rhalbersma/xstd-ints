//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div.hpp> // div
#include <limits>                    // numeric_limits

// The minimum divided by minus one is the maximum plus one, which the type cannot hold.
auto main() -> int
{
        auto const volatile denom = -1;
        static_cast<void>(xstd::div(std::numeric_limits<int>::min(), denom));
}
