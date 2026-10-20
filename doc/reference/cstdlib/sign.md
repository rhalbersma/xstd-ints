# `<xstd/ints/cstdlib/sign.hpp>`

## `sign`

- *Returns:* `-1` if `x` is negative, `0` if it is zero, and `+1` if it is positive, so `0` or `+1` over an unsigned
  type.
- It is total, `min()` and `max()` included, and a call is a constant expression.
- The result has type `int` whatever `I` is.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
