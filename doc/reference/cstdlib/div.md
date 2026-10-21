# `<xstd/ints/cstdlib/div.hpp>`

## `xstd::div`

```cpp
template<integer I>
  constexpr auto div(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
```

The quotient and remainder of truncated integer division, at the operands' own width.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `numer` | the numerator |
| `denom` | the denominator |

*Preconditions:* `denom != 0`, and for a signed `I`, not both `numer == xstd::numeric_limits<I>::min()` and
`denom == -1`. Each is undefined behaviour that `/` and `%` already have.

The second precondition does not apply to an unsigned `I`, where `min()` is `0` and `-1` converts to `max()`:
`div(0, max())` is `{0, 0}`.

### Return value

*Returns:* `{.quotient = numer / denom, .remainder = numer % denom}`, the truncated division of
[[expr.mul]](https://eel.is/c++draft/expr.mul): the quotient rounds toward zero,
`numer == denom * quotient + remainder`, `|remainder| < |denom|`, and a nonzero remainder has the sign of `numer`.

### Exceptions

The exception specification is `nothrow_const_operators<I>`.

### Remarks

It generalizes `div`, `ldiv`, `lldiv` and `imaxdiv` ([[cstdlib.syn]](https://eel.is/c++draft/cstdlib.syn), `constexpr`
since [P0533R9](https://github.com/cplusplus/papers/issues/104)) to every `integer`, `xstd::int128` among them, which
`div` does not take, and returns `div_result<I>` at the operands' own width rather than at a promoted one.

### Notes

- A call is a constant expression wherever `I`'s own `/` and `%` are.
- Both operands have the one type `I`: a literal divisor beside an integer-class numerator deduces `int` for it, and the
  call is ill-formed rather than converted.

### See also

| Entity | Relation |
| :----- | :------- |
| [`div_result`](div_result.md) | the result type |
| [`div_euclid`](div_euclid.md), [`div_floor`](div_floor.md), [`div_ceil`](div_ceil.md) | the other roundings |
| [`std::div`](https://en.cppreference.com/w/cpp/numeric/math/div) | the standard functions it generalizes |
