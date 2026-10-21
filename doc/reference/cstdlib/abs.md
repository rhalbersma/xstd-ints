# `<xstd/ints/cstdlib/abs.hpp>`

## `xstd::abs`

```cpp
template<integer I>
  constexpr auto abs(I x) noexcept(nothrow_const_operators<I>) -> I;
```

The magnitude of an integer, as a value of its own type.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | an integer value |

*Constraints:* `I` models `integer`.

*Preconditions:* For a signed `I`, `x != xstd::numeric_limits<I>::min()`, whose negation `I` cannot hold.

### Return value

*Returns:* `x` if it is not negative, and `-x` otherwise; over an unsigned type, `x`.

### Exceptions

The exception specification is `nothrow_const_operators<I>`.

### Remarks

It generalizes `abs`, `labs`, `llabs` and `imaxabs` ([[c.math.abs]](https://eel.is/c++draft/c.math.abs), `constexpr`
since [P0533R9](https://github.com/cplusplus/papers/issues/104)) to every `integer`, `xstd::int128` among them, and
returns `I` rather than the promoted type: the magnitude of an `int8_t` is an `int8_t`.

### Notes

A call is a constant expression.

### See also

| Entity | Relation |
| :----- | :------- |
| [`unsigned_abs`](unsigned_abs.md) | the magnitude as the unsigned counterpart, total over `min()` |
| [`sign`](sign.md) | the sign the magnitude leaves out |
| [`std::abs`](https://en.cppreference.com/w/cpp/numeric/math/abs) | the standard functions it generalizes |
