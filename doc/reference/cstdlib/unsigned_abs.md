# `<xstd/ints/cstdlib/unsigned_abs.hpp>`

## `xstd::unsigned_abs`

```cpp
template<integer I>
  constexpr auto unsigned_abs(I x) noexcept(nothrow_const_operators<I>) -> xstd::make_unsigned_t<I>;
```

The magnitude of an integer, as its unsigned counterpart. It is the counterpart of Rust's `unsigned_abs`, which the
standard library has none of.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | an integer value |

### Return value

*Returns:* The magnitude of `x` as `xstd::make_unsigned_t<I>`: for a signed `I`, `U(0) - U(x)` if `x` is negative and
`U(x)` otherwise, where `U` is that type; over an unsigned type, `x`.

### Exceptions

The exception specification is `nothrow_const_operators<I>`.

### Notes

- It is total: the magnitude of `min()` is `U(max()) + 1`, which no signed width holds and the unsigned counterpart
  does. A call is a constant expression.
- Over an unsigned type the result type is `I` itself.
- Over a signed type the result type is the unsigned counterpart.

### See also

| Entity | Relation |
| :----- | :------- |
| [`abs`](abs.md) | the magnitude as `I`, with `min()` outside its domain |
| [`make_unsigned`](../type_traits/make_unsigned.md) | names the result type |
| [`sign`](sign.md) | the sign the magnitude leaves out |
