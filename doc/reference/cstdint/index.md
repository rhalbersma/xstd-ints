# `<xstd/ints/cstdint.hpp>`

```cpp
#define XSTD_HAS_INT128 see below
#define XSTD_HAS_BIT_INT see below

namespace xstd {
  // where XSTD_HAS_INT128 is defined
  using int128 = see below;
  using uint128 = see below;
  template<> struct make_unsigned<int128>;
  template<> struct make_signed<uint128>;

  // where XSTD_HAS_BIT_INT is defined
  inline constexpr size_t bit_int_max_width = __BITINT_MAXWIDTH__;
  template<size_t N>
    requires (2 <= N and N <= bit_int_max_width)
    using bit_int = signed _BitInt(N);
  template<size_t N>
    requires (2 <= N and N <= bit_int_max_width)
    using bit_uint = unsigned _BitInt(N);
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/cstdint/bit_int.hpp>`](bit_int.md) | `bit_int`, `bit_uint` |
| [`<xstd/ints/cstdint/int128.hpp>`](int128.md) | `int128`, `uint128` |

The umbrella exports `<xstd/ints/cstdint/int128.hpp>` and `<xstd/ints/cstdint/bit_int.hpp>`. Every type it names is half
of a pair: each transformation leads to the other half and back, and the unsigned half has one more digit than the
signed half, cv-qualified or not.

`<xstd/ints/cstdint/bit_int.hpp>` exports `<xstd/ints/limits/numeric_limits.hpp>`,
`<xstd/ints/type_traits/make_signed.hpp>` and `<xstd/ints/type_traits/make_unsigned.hpp>`, which declare the `_BitInt`
specializations of `numeric_limits`, `make_signed` and `make_unsigned`, so including it is enough to ask them of the
aliases.
