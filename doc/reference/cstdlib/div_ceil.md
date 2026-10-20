# `<xstd/ints/cstdlib/div_ceil.hpp>`

## `div_ceil`

- *Preconditions:* As for `div`.
- *Returns:* For a signed `I`, `{.quotient = q, .remainder = numer - denom * q}`, where `q` is `numer / denom` rounded
  toward positive infinity: `|remainder| < |denom|`, and a nonzero remainder has the sign opposite to `denom`'s.
- *Returns:* For an unsigned `I`, the same `q`, with `numer - denom * q` reduced modulo 2<sup>N</sup> for the width N of
  `I` as the remainder, as [P3724R4](https://github.com/cplusplus/papers/issues/2354)'s `div_rem_to_pos_inf` words it: an inexact quotient still rounds up, and
  `numer == denom * quotient + remainder` holds in `I`'s own arithmetic.
- The quotient is one more than `div_floor`'s wherever that leaves a remainder, and equal to it elsewhere.
- A call is a constant expression wherever `I`'s own `/` and `%` are.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
