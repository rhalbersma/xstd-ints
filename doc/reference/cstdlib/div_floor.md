# `<xstd/ints/cstdlib/div_floor.hpp>`

## `div_floor`

- *Preconditions:* As for `div`.
- *Returns:* `{.quotient = q, .remainder = numer - denom * q}`, where `q` is `numer / denom` rounded toward negative
  infinity: `|remainder| < |denom|`, and a nonzero remainder has the sign of `denom`.
- Over an unsigned `I` it is `div`, and over a signed one it agrees with `div` wherever the division is exact.
- A call is a constant expression wherever `I`'s own `/` and `%` are.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
