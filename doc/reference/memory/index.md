# `<xstd/ints/memory.hpp>`

```cpp
namespace xstd {
  template<alignable T>
    constexpr auto align_down(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> T;
  template<class T>
    auto align_down(T* pointer, size_t alignment) noexcept -> T*;
  template<alignable T>
    constexpr auto align_up(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> T;
  template<class T>
    auto align_up(T* pointer, size_t alignment) noexcept -> T*;
  template<alignable T>
    constexpr auto is_aligned(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> bool;
  template<class T>
    auto is_aligned(T* pointer, size_t alignment) noexcept -> bool;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/memory/align_down.hpp>`](align_down.md) | `align_down` |
| [`<xstd/ints/memory/align_up.hpp>`](align_up.md) | `align_up` |
| [`<xstd/ints/memory/is_aligned.hpp>`](is_aligned.md) | `is_aligned` |

The umbrella exports the three headers under `<xstd/ints/memory/>`. The three agree with [Boost.Align](https://www.boost.org/doc/libs/latest/libs/align/) on values at every
standard unsigned width and on pointers, and what `align_up` and `align_down` return satisfies `is_aligned`.
