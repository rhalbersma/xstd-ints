//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_repeat.hpp>   // bit_repeat
#include <xstd/ints/ext/boost/int128.hpp> // uint128
#include <boost/int128.hpp>               // uint128

// A period of zero, read as no repetition at all; <bit> declines this type wherever it has bit_repeat.
auto main() -> int
{
        auto const volatile period = 0;
        static_cast<void>(xstd::bit_repeat(boost::int128::uint128{0b101U}, period));
}
