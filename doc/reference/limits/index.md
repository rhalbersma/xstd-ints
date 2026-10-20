# `<xstd/ints/limits.hpp>`

```cpp
namespace xstd {
  template<class T>
    struct numeric_limits {};
  template<class T>
    requires (is_object_v<T> and not is_array_v<T>)
    struct numeric_limits<T> : std::numeric_limits<T> {};
  template<class T>
    struct numeric_limits<const T> : numeric_limits<T> {};
  template<class T>
    struct numeric_limits<volatile T> : numeric_limits<T> {};
  template<class T>
    struct numeric_limits<const volatile T> : numeric_limits<T> {};

  // where __BITINT_MAXWIDTH__ is defined
  template<size_t N> struct numeric_limits<signed _BitInt(N)>;
  template<size_t N> struct numeric_limits<unsigned _BitInt(N)>;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/limits/numeric_limits.hpp>`](numeric_limits.md) | `numeric_limits` |

The umbrella exports `<xstd/ints/limits/numeric_limits.hpp>`.
