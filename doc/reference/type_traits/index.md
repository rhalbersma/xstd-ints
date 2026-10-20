# `<xstd/ints/type_traits.hpp>`

```cpp
namespace xstd {
  template<class T>
    constexpr bool is_signed_v = see below;
  template<class T>
    using is_signed = bool_constant<is_signed_v<T>>;
  template<class T>
    constexpr bool is_unsigned_v = see below;
  template<class T>
    using is_unsigned = bool_constant<is_unsigned_v<T>>;

  template<class T>
    struct make_signed;
  template<class T>
    using make_signed_t = typename make_signed<T>::type;
  template<class T>
    struct make_unsigned;
  template<class T>
    using make_unsigned_t = typename make_unsigned<T>::type;

  // where __BITINT_MAXWIDTH__ is defined
  template<> struct make_signed<unsigned _BitInt(1)>;
  template<size_t N> struct make_signed<unsigned _BitInt(N)>;
  template<> struct make_unsigned<unsigned _BitInt(1)>;
  template<size_t N> struct make_unsigned<signed _BitInt(N)>;

  template<class T>
    using promoted_t = see below;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/type_traits/is_signed.hpp>`](is_signed.md) | `is_signed` |
| [`<xstd/ints/type_traits/is_unsigned.hpp>`](is_unsigned.md) | `is_unsigned` |
| [`<xstd/ints/type_traits/make_signed.hpp>`](make_signed.md) | `make_signed` |
| [`<xstd/ints/type_traits/make_unsigned.hpp>`](make_unsigned.md) | `make_unsigned` |
| [`<xstd/ints/type_traits/promoted.hpp>`](promoted.md) | `promoted` |

The umbrella exports the five headers under `<xstd/ints/type_traits/>`. Over every exact-width type, an integer is in
exactly one half, each modifier lands in the half it names and the two are mutually inverse, and promotion keeps a
signed type signed.
