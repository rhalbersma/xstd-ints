//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_MEMORY_ALIGN_DOWN_HPP
#define XSTD_INTS_MEMORY_ALIGN_DOWN_HPP

#include <xstd/ints/concepts/alignable.hpp> // alignable, nothrow_alignable
#include <bit>                              // has_single_bit
#include <cassert>                          // assert
#include <cstddef>                          // size_t
#include <cstdint>                          // uintptr_t

namespace xstd {

// Rounds value down to the previous multiple of a power-of-two alignment.
template<alignable T>
[[nodiscard]] constexpr auto align_down(T value, std::size_t alignment) noexcept(nothrow_alignable<T>)
        -> T
{
        assert(std::has_single_bit(alignment));
        auto const mask = static_cast<T>(alignment - 1);
        // Subtracting the remainder is what alignment is; & mask is only how a radix-2 type spells % alignment.
        return static_cast<T>(value - static_cast<T>(value & mask));
}

// The same arithmetic in the address space, delegating rather than repeating the mask.
template<class T>
[[nodiscard]] auto align_down(T* pointer, std::size_t alignment) noexcept
        -> T*
{
        // NOLINTNEXTLINE(performance-no-int-to-ptr): the aligned address has to become a pointer again
        return reinterpret_cast<T*>(xstd::align_down(reinterpret_cast<std::uintptr_t>(pointer), alignment));
}

} // namespace xstd

#endif // XSTD_INTS_MEMORY_ALIGN_DOWN_HPP
