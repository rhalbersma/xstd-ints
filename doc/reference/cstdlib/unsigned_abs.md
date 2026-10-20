# `<xstd/ints/cstdlib/unsigned_abs.hpp>`

## `unsigned_abs`

- *Returns:* The magnitude of `x` as `xstd::make_unsigned_t<I>`: for a signed `I`, `U(0) - U(x)` if `x` is negative and
  `U(x)` otherwise, where `U` is that type; over an unsigned type, `x`. It is the counterpart of Rust's `unsigned_abs`,
  which the standard library has none of.
- It is total: the magnitude of `min()` is `U(max()) + 1`, which no signed width holds and the unsigned counterpart
  does. A call is a constant expression.
- Over an unsigned type the result type is `I` itself.
- Over a signed type the result type is the unsigned counterpart.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
