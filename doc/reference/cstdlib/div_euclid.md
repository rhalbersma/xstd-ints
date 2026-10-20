# `<xstd/ints/cstdlib/div_euclid.hpp>`

## `xstd::div_euclid`

```cpp
template<integer I>
  constexpr auto div_euclid(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
```

The quotient and remainder of Euclidean integer division, whose remainder is never negative.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `numer` | the numerator |
| `denom` | the denominator |

*Preconditions:* As for `div`.

### Return value

*Returns:* `{.quotient = q, .remainder = numer - denom * q}`, where `q` is the quotient for which
`0 <= remainder < |denom|`: Euclidean division.

### Exceptions

The exception specification is `nothrow_const_operators<I>`.

### Notes

- Over an unsigned `I` it is `div`, and over a signed one it agrees with `div` wherever the division is exact.
- A call is a constant expression wherever `I`'s own `/` and `%` are.

### See also

| Entity | Relation |
| :----- | :------- |
| [`div_result`](div_result.md) | the result type |
| [`div`](div.md), [`div_floor`](div_floor.md), [`div_ceil`](div_ceil.md) | the other roundings |
