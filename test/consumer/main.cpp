//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer.hpp>
#include <xstd/ints/cstdlib.hpp>
#include <xstd/ints/format.hpp>
#include <format>

auto main()
        -> int
{
        static_assert(xstd::integer<int>);
        static_assert(xstd::div_euclid(-8, 3).remainder == 1);

        auto const d = xstd::div_floor(-8, 3);
        return std::format("{}", d) == "(-3, 1)" ? 0 : 1;
}
