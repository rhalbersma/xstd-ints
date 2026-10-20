# `<xstd/ints/cstdlib/div_result.hpp>`

## `div_result`

```cpp
template<integer I>
struct div_result {
  I quotient, remainder;

  friend auto operator==(const div_result&, const div_result&) -> bool = default;
};
```

- An aggregate of two members of type `I`, `quotient` then `remainder`, which designated initializers and a structured
  binding name in that order. It is usable in constant expressions.
- *Remarks:* The members carry [P3724R4](https://github.com/cplusplus/papers/issues/2354)'s names where `div_t`, `ldiv_t`, `lldiv_t` and `imaxdiv_t` have `quot` and `rem`,
  and one template serves every `integer`.
- *Remarks:* It declares no `tuple_size`, `tuple_element` or `get`, so it is not tuple-like: a structured binding
  decomposes its public members, and `apply` does not take it.
- `I` is constrained to model `integer`, so a character type and `bool` are turned away.
- The deduction guide deduces `I` from two arguments of one type, signed or unsigned, at every standard width.
- `operator==`:
  - *Returns:* `x.quotient == y.quotient and x.remainder == y.remainder`, by defaulted memberwise comparison, in a
    constant expression as well.
  - *Remarks:* The exception specification is deduced, and is that of `I`'s own `==`.
