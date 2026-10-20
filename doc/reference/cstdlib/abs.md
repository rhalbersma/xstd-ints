# `<xstd/ints/cstdlib/abs.hpp>`

## `abs`

- *Constraints:* `I` models `integer`.
- *Preconditions:* For a signed `I`, `x != xstd::numeric_limits<I>::min()`, whose negation `I` cannot hold.
- *Returns:* `x` if it is not negative, and `-x` otherwise; over an unsigned type, `x`.
- A call is a constant expression.
- *Remarks:* It generalizes `abs`, `labs`, `llabs` and `imaxabs` ([[c.math.abs]](https://eel.is/c++draft/c.math.abs), `constexpr` since
  [P0533R9](https://github.com/cplusplus/papers/issues/104)) to every `integer`, `xstd::int128` among them, and returns `I` rather than the
  promoted type: the magnitude of an `int8_t` is an `int8_t`.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
