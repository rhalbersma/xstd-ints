# `<xstd/ints/cstdlib/sign.hpp>`

## `xstd::sign`

```cpp
template<integer I>
  constexpr auto sign(I x) noexcept(nothrow_const_operators<I>) -> int;
```

The sign of an integer, as an `int`.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | an integer value |

### Return value

*Returns:* `-1` if `x` is negative, `0` if it is zero, and `+1` if it is positive, so `0` or `+1` over an unsigned
type.

### Exceptions

The exception specification is `nothrow_const_operators<I>`.

### Notes

- It is total, `min()` and `max()` included, and a call is a constant expression.
- The result has type `int` whatever `I` is.

### See also

| Entity | Relation |
| :----- | :------- |
| [`abs`](abs.md), [`unsigned_abs`](unsigned_abs.md) | the magnitude the sign leaves out |
