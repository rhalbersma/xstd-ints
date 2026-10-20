# `<xstd/ints/cstdint/bit_int.hpp>`

## `XSTD_HAS_BIT_INT`

```cpp
#define XSTD_HAS_BIT_INT see below
```

Whether the bit-precise integer types are available. Defined as `1` exactly where the compiler defines
`__BITINT_MAXWIDTH__`, which among the supported compilers only Clang does in C++.

### See also

| Entity | Relation |
| :----- | :------- |
| [`bit_int`, `bit_uint`](#xstdbit_int-xstdbit_uint) | declared where it is defined |
| [`XSTD_HAS_INT128`](int128.md#xstd_has_int128) | the same question asked of the 128-bit pair |

## `xstd::bit_int_max_width`

```cpp
inline constexpr size_t bit_int_max_width = __BITINT_MAXWIDTH__;
```

The widest bit-precise integer the compiler declares: `__BITINT_MAXWIDTH__`, as a `size_t`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`bit_int`, `bit_uint`](#xstdbit_int-xstdbit_uint) | are constrained on it |

## `xstd::bit_int`, `xstd::bit_uint`

```cpp
// (1)
template<size_t N>
  requires (2 <= N and N <= bit_int_max_width)
using bit_int = signed _BitInt(N);

// (2)
template<size_t N>
  requires (2 <= N and N <= bit_int_max_width)
using bit_uint = unsigned _BitInt(N);
```

The bit-precise integer types of width `N`, by name.

1. `bit_int<N>` is `signed _BitInt(N)`.
2. `bit_uint<N>` is `unsigned _BitInt(N)`.

### Template parameters

| Parameter | Meaning |
| :-------- | :------ |
| `N` | The width in bits. |

*Constraints:* `2 <= N` and `N <= bit_int_max_width`. The lower bound pairs every exposed width: Clang does not yet
implement [WG14 N3747](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3747.pdf)'s `signed _BitInt(1)`. Both aliases
name a type at 2 and at `bit_int_max_width`, and neither at 1 or at `bit_int_max_width + 1`.

### Remarks

Clang's `-Wbit-int-extension` is suppressed within the header, and within each of the three headers that declare a
`_BitInt` specialization.

### Notes

- They are aliases, not wrappers, so the conversions, promotions and operators are the compiler's.
- `bit_int<17>` models `signed_integer` and `bit_uint<17>` models `unsigned_integer`, and each is the other's
  counterpart.
- At widths 2, 17 and 64 the two are a pair, and `bit_int<17> const` pairs with `bit_uint<17> const`.
- `bit_int<17>` and `bit_uint<17>` model `nothrow_const_operators`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`numeric_limits<signed _BitInt(N)>`](../limits/numeric_limits.md#xstdnumeric_limitssigned-_bitintn-xstdnumeric_limitsunsigned-_bitintn) | their limits, at the requested width |
| [`make_signed<unsigned _BitInt(N)>`](../type_traits/make_signed.md#xstdmake_signedunsigned-_bitintn-xstdmake_unsignedsigned-_bitintn) | pairs each with the other |
| [`int128`, `uint128`](int128.md) | the 128-bit pair |
