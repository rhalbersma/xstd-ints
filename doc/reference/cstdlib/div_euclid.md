# `<xstd/ints/cstdlib/div_euclid.hpp>`

## `div_euclid`

- *Preconditions:* As for `div`.
- *Returns:* `{.quotient = q, .remainder = numer - denom * q}`, where `q` is the quotient for which
  `0 <= remainder < |denom|`: Euclidean division.
- Over an unsigned `I` it is `div`, and over a signed one it agrees with `div` wherever the division is exact.
- A call is a constant expression wherever `I`'s own `/` and `%` are.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
