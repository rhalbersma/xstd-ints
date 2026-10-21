# `<xstd/ints/cstdlib/div_result.hpp>`

## `xstd::div_result`

```cpp
// (1)
template<integer I>
struct div_result {
  I quotient, remainder;

  friend auto operator==(const div_result&, const div_result&) -> bool = default;
};

// (2)
template<integer I>
  div_result(I, I) -> div_result<I>;
```

The result of an integer division: an aggregate of two members of type `I`, `quotient` then `remainder`, which
designated initializers and a structured binding name in that order.

1. The class template.
2. The deduction guide, which deduces `I` from two arguments of one type, signed or unsigned, at every standard width.

### Template parameters

| Parameter | Meaning |
| :-------- | :------ |
| `I` | The type of both members. Constrained to model `integer`, so a character type and `bool` are turned away. |

### Member functions

| Function | Purpose |
| :------- | :------ |
| [`operator==`](#operator) | compares memberwise |

#### `operator==`

```cpp
friend auto operator==(const div_result&, const div_result&) -> bool = default;
```

- *Returns:* `x.quotient == y.quotient and x.remainder == y.remainder`, by defaulted memberwise comparison, in a
  constant expression as well.
- *Remarks:* The exception specification is deduced, and is that of `I`'s own `==`.

### Remarks

- The members carry [P3724R4](https://github.com/cplusplus/papers/issues/2354)'s names where `div_t`, `ldiv_t`,
  `lldiv_t` and `imaxdiv_t` have `quot` and `rem`, and one template serves every `integer`.
- It declares no `tuple_size`, `tuple_element` or `get`, so it is not tuple-like: a structured binding decomposes its
  public members, and `apply` does not take it.

### Notes

It is usable in constant expressions.

### See also

| Entity | Relation |
| :----- | :------- |
| [`div`](div.md), [`div_euclid`](div_euclid.md), [`div_floor`](div_floor.md), [`div_ceil`](div_ceil.md) | return it |
| [`formatter<div_result>`](../format/div_result.md) | formats it |
| [`std::div_t`](https://en.cppreference.com/w/cpp/numeric/math/div) | the standard's result types, whose members are `quot` and `rem` |
