//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/detail/portable/block_compress.hpp> // block_compress_by_rank
#include <xstd/ints/bit/detail/portable/block_expand.hpp>   // block_expand_by_select, block_expand_by_walk
#include <xstd/ints/bit/detail/portable/block_reverse.hpp>  // block_reverse_by_swaps
#include <xstd/ints/bit/detail/portable/block_select.hpp>   // block_select_by_broadword
#include <bit>                                              // popcount
#include <cstddef>                                          // size_t
#include <iostream>                                         // cin, cout
#include <limits>                                           // numeric_limits
#include <string_view>                                      // string_view

// The compiled portable kernels on each pair of hexadecimal words read, for the models of them to be held against.
auto main(int argc, char* argv[]) -> int
{
        if (argc == 2 and std::string_view(argv[1]) == "--width") {
                std::cout << std::numeric_limits<std::size_t>::digits << '\n';
                return 0;
        }
        namespace portable = xstd::ints::detail::portable;
        std::cin >> std::hex;
        std::cout << std::hex;
        for (auto x = 0UZ, y = 0UZ; std::cin >> x >> y;) {
                std::cout << portable::block_reverse_by_swaps(x) << ' ' << portable::block_compress_by_rank(x, y) << ' ' << portable::block_expand_by_walk(x, y) << ' '
                          << portable::block_expand_by_select(x, y) << ' ';
                // The rank the second word names among the first word's ones, so the precondition always holds.
                if (x == 0UZ) {
                        std::cout << "-\n";
                } else {
                        std::cout << portable::block_select_by_broadword(x, static_cast<unsigned>(y % static_cast<std::size_t>(std::popcount(x)))) << '\n';
                }
        }
        return 0;
}
