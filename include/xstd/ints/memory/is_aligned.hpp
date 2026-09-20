//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_MEMORY_IS_ALIGNED_HPP
#define XSTD_INTS_MEMORY_IS_ALIGNED_HPP

#include <xstd/ints/concepts/alignable.hpp> // alignable, nothrow_alignable
#include <bit>                              // has_single_bit
#include <cassert>                          // assert
#include <cstddef>                          // size_t
#include <cstdint>                          // uintptr_t

namespace xstd {

// Whether value is already a multiple of a power-of-two alignment, which is its remainder being zero.
template<alignable T>
[[nodiscard]] constexpr auto is_aligned(T value, std::size_t alignment) noexcept(nothrow_alignable<T>)
        -> bool
{
        assert(std::has_single_bit(alignment));
        auto const mask = static_cast<T>(alignment - 1);
        // Zero through the size_t conversion alignable asks for, T{} needing a constructor it never requires.
        return static_cast<T>(value & mask) == static_cast<T>(0UZ);
}

// The same question in the address space, delegating rather than repeating the mask; qualified as cstdlib's calls are.
template<class T>
[[nodiscard]] auto is_aligned(T* pointer, std::size_t alignment) noexcept
        -> bool
{
        return xstd::is_aligned(reinterpret_cast<std::uintptr_t>(pointer), alignment);
}

} // namespace xstd

#endif // XSTD_INTS_MEMORY_IS_ALIGNED_HPP
