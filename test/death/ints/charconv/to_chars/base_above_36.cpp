//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/charconv/to_chars.hpp> // to_chars
#include <array>                           // array

// Base 64 is an encoding with its own alphabet, not a radix that digits and letters can spell.
auto main() -> int
{
        auto buffer              = std::array<char, 16>{};
        auto const volatile base = 64;
        static_cast<void>(xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), 255, base));
}
