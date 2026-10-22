//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory/align_up.hpp> // align_up
#include <cstddef>                       // size_t
#include <limits>                        // numeric_limits

// The maximum, a not-found sentinel such as npos, has no multiple of 8 above it.
auto main() -> int
{
        auto const volatile value = std::numeric_limits<std::size_t>::max();
        static_cast<void>(xstd::align_up(value, 8));
}
