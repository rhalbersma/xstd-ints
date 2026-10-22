//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cases.hpp>           // cases, inputs
#include <valgrind/memcheck.h> // VALGRIND_MAKE_MEM_DEFINED, VALGRIND_MAKE_MEM_UNDEFINED
#include <cstdint>             // uint64_t
#include <cstdio>              // fprintf, printf
#include <random>              // mt19937_64
#include <string_view>         // string_view

// ctgrind's method: the secret is marked undefined, so memcheck reports any branch or address that depends on it.
auto main(int argc, char** argv) -> int
{
        if (argc != 2) {
                std::fprintf(stderr, "usage: %s kernel.secret\n", argv[0]);
                return 2;
        }
        for (auto const& c : constant_time::cases) {
                if (c.name != std::string_view{argv[1]}) {
                        continue;
                }
                auto rng = std::mt19937_64{20261017U};
                auto sum = std::uint64_t{0};
                for (auto i = 0; i < 256; ++i) {
                        auto const first  = rng();
                        auto const second = rng();
                        auto in           = (i % 2 == 0) ? c.random(first, second) : c.fixed(first, second);
                        if (c.a_secret) {
                                VALGRIND_MAKE_MEM_UNDEFINED(&in.a, sizeof(in.a));
                        }
                        if (c.b_secret) {
                                VALGRIND_MAKE_MEM_UNDEFINED(&in.b, sizeof(in.b));
                        }
                        auto out = c.run(in);
                        // The result is as secret as its inputs, but only how it was computed is under test.
                        VALGRIND_MAKE_MEM_DEFINED(&out, sizeof(out));
                        sum ^= out;
                }
                std::printf("%s: %#llx\n", argv[1], static_cast<unsigned long long>(sum));
                return 0;
        }
        std::fprintf(stderr, "no kernel case named %s\n", argv[1]);
        return 2;
}
