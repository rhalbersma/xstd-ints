//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/bit_compress.hpp> // bit_compress
#include <xstd/ints/bit/bit_expand.hpp>   // bit_expand
#include <xstd/ints/bit/bit_reverse.hpp>  // bit_reverse
#include <xstd/ints/bit/bit_select.hpp>   // bit_select
#include <xstd/ints/cstdint.hpp>          // uint128
#include <cstdint>                        // uint64_t

// Each function is compiled once per target configuration, and its assembly read against the directives above it.
extern "C" {

// CHECK-LABEL: codegen_bit_compress:
// PORTABLE-NOT: {{^\s+(pext|pdep)\s}}
// FAST-BMI2-NOT: {{^\s+(j[a-z]+|call)\s}}
// FAST-BMI2: {{^\s+pext\s}}
// FAST-BMI2-NOT: {{^\s+(j[a-z]+|call)\s}}
[[nodiscard]] auto codegen_bit_compress(std::uint64_t src, std::uint64_t mask)
        -> std::uint64_t
{
        return xstd::bit_compress(src, mask);
}

// CHECK-LABEL: codegen_bit_expand:
// PORTABLE-NOT: {{^\s+(pext|pdep)\s}}
// FAST-BMI2-NOT: {{^\s+(j[a-z]+|call)\s}}
// FAST-BMI2: {{^\s+pdep\s}}
// FAST-BMI2-NOT: {{^\s+(j[a-z]+|call)\s}}
[[nodiscard]] auto codegen_bit_expand(std::uint64_t src, std::uint64_t mask)
        -> std::uint64_t
{
        return xstd::bit_expand(src, mask);
}

// TZCNT is BMI1's, so BMI2 alone leaves a zero test after the PDEP.
// CHECK-LABEL: codegen_bit_select:
// PORTABLE-NOT: {{^\s+(pext|pdep)\s}}
// PDEP-SELECT: {{^\s+pdep\s}}
// TZCNT-NOT: {{^\s+(j[a-z]+|call)\s}}
// TZCNT: {{^\s+pdep\s}}
// TZCNT-NEXT: {{^\s+tzcnt\s}}
// TZCNT-NOT: {{^\s+(j[a-z]+|call)\s}}
// TZCNT: {{^\s+ret}}
[[nodiscard]] auto codegen_bit_select(std::uint64_t x, int k)
        -> int
{
        return xstd::bit_select(x, k);
}

// CHECK-LABEL: codegen_bit_reverse:
// GFNI-NOT: {{^\s+(j[a-z]+|call)\s}}
// GFNI: {{^\s+v?gf2p8affineqb\s}}
// GFNI: {{^\s+bswap\s}}
// GFNI-NOT: {{^\s+(j[a-z]+|call)\s}}
// A64-NOT: {{^\s+(b|bl|br|b\.[a-z]+|cbn?z|tbn?z)\s}}
// A64: {{^\s+rbit\s}}
// A64-NOT: {{^\s+(b|bl|br|b\.[a-z]+|cbn?z|tbn?z)\s}}
[[nodiscard]] auto codegen_bit_reverse(std::uint64_t x)
        -> std::uint64_t
{
        return xstd::bit_reverse(x);
}

// Two blocks, each mirrored once and moved to the other's offset, with nothing left of the loop over them.
// CHECK-LABEL: codegen_bit_reverse_128:
// X86-STRAIGHT-NOT: {{^\s+(j[a-z]+|call)\s}}
// GFNI: {{^\s+v?gf2p8affineqb\s}}
// GFNI: {{^\s+v?gf2p8affineqb\s}}
// X86-STRAIGHT-NOT: {{^\s+(j[a-z]+|call)\s}}
// A64-NOT: {{^\s+(b|bl|br|b\.[a-z]+|cbn?z|tbn?z)\s}}
// A64: {{^\s+rbit\s}}
// A64: {{^\s+rbit\s}}
// A64-NOT: {{^\s+(b|bl|br|b\.[a-z]+|cbn?z|tbn?z)\s}}
[[nodiscard]] auto codegen_bit_reverse_128(xstd::uint128 x)
        -> xstd::uint128
{
        return xstd::bit_reverse(x);
}

} // extern "C"
