//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/charconv/to_chars.hpp> // to_chars
#include <array>                           // array

// Base zero, which strtol reads as a request to detect the base, is no radix to write in.
auto main() -> int
{
        auto buffer              = std::array<char, 16>{};
        auto const volatile base = 0;
        static_cast<void>(xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), 255, base));
}
