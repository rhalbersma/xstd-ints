# `<xstd/ints/cstdint/bit_int.hpp>`

## `XSTD_HAS_BIT_INT`

- Defined as `1` exactly where the compiler defines `__BITINT_MAXWIDTH__`, which among the supported compilers only Clang
  does in C++.

## `bit_int_max_width`

- `__BITINT_MAXWIDTH__`, as a `size_t`.

## `bit_int`, `bit_uint`

- *Constraints:* `2 <= N` and `N <= bit_int_max_width`. The lower bound pairs every exposed width: Clang does not yet
  implement [WG14 N3747](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3747.pdf)'s `signed _BitInt(1)`. Both aliases name a type at 2 and at `bit_int_max_width`, and neither at 1 or
  at `bit_int_max_width + 1`.
- `bit_int<N>` is `signed _BitInt(N)` and `bit_uint<N>` is `unsigned _BitInt(N)`: aliases, not wrappers, so the
  conversions, promotions and operators are the compiler's.
- `bit_int<17>` models `signed_integer` and `bit_uint<17>` models `unsigned_integer`, and each is the other's
  counterpart.
- At widths 2, 17 and 64 the two are a pair, and `bit_int<17> const` pairs with `bit_uint<17> const`.
- `bit_int<17>` and `bit_uint<17>` model `nothrow_const_operators`.
- *Remarks:* Clang's `-Wbit-int-extension` is suppressed within the header, and within each of the three headers that
  declare a `_BitInt` specialization.
