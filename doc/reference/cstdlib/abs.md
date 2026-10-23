# `<xstd/ints/cstdlib/abs.hpp>`

## `xstd::abs`

```cpp
template<integer I>
  constexpr auto abs(I x) noexcept(nothrow_const_operators<I>) -> promoted_t<I>;
```

The magnitude of an integer, in the type `std::abs` gives: its promoted type, `int` for a type narrower than `int`.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | an integer value |

*Constraints:* `I` models `integer`.

*Preconditions:* For a signed `promoted_t<I>`, `x != xstd::numeric_limits<promoted_t<I>>::min()`, whose negation
`promoted_t<I>` cannot hold. Below `int` no value violates it: `abs` of `int8_t`'s minimum is `128`.

### Return value

*Returns:* `x` converted to `promoted_t<I>` if it is not negative, and the negation of that otherwise; over an
unsigned `promoted_t<I>`, `x` converted to it.

### Exceptions

The exception specification is `nothrow_const_operators<I>`.

### Remarks

It generalizes `abs`, `labs`, `llabs` and `imaxabs` ([[c.math.abs]](https://eel.is/c++draft/c.math.abs), `constexpr`
since [P0533R9](https://github.com/cplusplus/papers/issues/104)) to every `integer`, `xstd::int128` among them, and
returns the type they return for every type they take: `int` for an `int8_t`, `short` or `unsigned char`, and the type
itself for `int` and wider, `xstd::int128` and `bit_int<N>`, which
[[conv.prom]](https://eel.is/c++draft/conv.prom) leaves as they are.

### Notes

A call is a constant expression.

### See also

| Entity | Relation |
| :----- | :------- |
| [`unsigned_abs`](unsigned_abs.md) | the magnitude as the unsigned counterpart of `I`, total over `min()` |
| [`promoted_t`](../type_traits/promoted.md) | the type it returns |
| [`sign`](sign.md) | the sign the magnitude leaves out |
| [`std::abs`](https://en.cppreference.com/w/cpp/numeric/math/abs) | the standard functions it generalizes |
