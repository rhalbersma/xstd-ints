//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_ceil.hpp>     // bit_ceil
#include <xstd/ints/ext/boost/int128.hpp> // uint128
#include <boost/int128.hpp>               // uint128

// One past the highest power of two rounds up to a power the width cannot hold; <bit> declines this type.
auto main() -> int
{
        auto const volatile above = 1U;
        auto const x              = (boost::int128::uint128{1} << 127U) + boost::int128::uint128{above};
        static_cast<void>(xstd::bit_ceil(x));
}
