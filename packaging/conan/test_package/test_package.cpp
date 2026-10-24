//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints.hpp> // bit_compress, div_floor
#include <cstdint>       // uint64_t
#include <cstdlib>       // EXIT_FAILURE, EXIT_SUCCESS

auto main() -> int
{
        auto const q = xstd::div_floor(-7, 2);
        auto const c = xstd::bit_compress(std::uint64_t{0b1010'1100}, std::uint64_t{0b1111'0000});
        return q.quotient == -4 and q.remainder == 1 and c == 0b1010 ? EXIT_SUCCESS : EXIT_FAILURE;
}
