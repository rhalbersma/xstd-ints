//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory/align_down.hpp> // align_down
#include <cstddef>                         // size_t

// A size of 24, three pointers wide, passed where an alignment is asked for.
auto main() -> int
{
        auto const volatile alignment = std::size_t{24};
        static_cast<void>(xstd::align_down(std::size_t{100}, alignment));
}
