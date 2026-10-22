//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory/align_down.hpp> // align_down
#include <xstd/ints/memory/align_up.hpp>   // align_up
#include <cstddef>                         // size_t
#include <cstdint>                         // uint64_t

// Each library function beside the idiom it declines, the high bits kept by complementing the mask: same instructions.
extern "C" {

[[nodiscard]] auto codegen_align_up(std::uint64_t value, std::size_t alignment)
        -> std::uint64_t
{
        return xstd::align_up(value, alignment);
}

[[nodiscard]] auto codegen_align_up_by_complement(std::uint64_t value, std::size_t alignment)
        -> std::uint64_t
{
        return (value + (alignment - 1)) & ~(alignment - 1);
}

[[nodiscard]] auto codegen_align_down(std::uint64_t value, std::size_t alignment)
        -> std::uint64_t
{
        return xstd::align_down(value, alignment);
}

[[nodiscard]] auto codegen_align_down_by_complement(std::uint64_t value, std::size_t alignment)
        -> std::uint64_t
{
        return value & ~(alignment - 1);
}

} // extern "C"
