//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div.hpp>       // div
#include <xstd/ints/cstdlib/div_floor.hpp> // div_floor
#include <xstd/ints/format/div_result.hpp> // formatter<div_result>
#include <cstdint>                         // int32_t, uint8_t
#include <format>                          // format
#include <iostream>                        // cout

auto main() -> int
{
        auto const truncated = xstd::div(std::int32_t{-7}, std::int32_t{2});
        auto const floored   = xstd::div_floor(std::int32_t{-7}, std::int32_t{2});

        // As a pair: in brackets, without them, or with a colon between.
        std::cout << std::format("{}", truncated) << '\n';
        std::cout << std::format("{:n}", truncated) << '\n';
        std::cout << std::format("{:m}", floored) << '\n';

        // Fill, alignment and width apply to the whole.
        std::cout << std::format("[{:*^12}]", floored) << '\n';

        // The members of a byte-sized type are written as numbers, not as characters.
        std::cout << std::format("{}", xstd::div(std::uint8_t{200}, std::uint8_t{7})) << '\n';
}
